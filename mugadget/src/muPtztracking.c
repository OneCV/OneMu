/* ------------------------------------------------------------------------- /
 *
 * Module: muPtztracking.c
 * Author: Joe Lin
 * Create date: 07/6/2012
 *
 * Description:
 *    This application is presented the PTZ Auto-Tracking.
 *
 -------------------------------------------------------------------------- */
 
#include "muGadget.h"

FILE *g_saveptr; 
FILE *g_saveptr2; 
FILE *g_saveptr3;

/* Macro & Pre-Define */
#define GAMMA_TH 0.2F
#define AIM_POSITION_X	4
#define AIM_POSITION_Y	4
#define AIM_POSITION_W	2
#define	AIM_POSITION_H	2
#define ENLARGE_WIDTH 2.5
#define ENLARGE_MINX 0.75
#define ENLARGE_HEIGHT 2.5
#define ENLARGE_MINY 0.75
#define STOP_FRAME	60
#define MIN_ZOOM_RATIO 10
#define MAX_Y_SPD 255
#define INI_TILT_SPEED 20
#define INI_PAN_SPEED 40

//smooth control x, y
#define NUMBERX 13
#define NUMBERY 7

#define SPD_COUNT 60

//simulation motion dection
#define FILTER_TH 20
enum
{
	PTZ_STOP=0,
	PTZ_PAN,
	PTZ_TILT_UP,
	PTZ_TILT_DOWN,
};

enum
{
	CURRNET_ZOOM=0,
	ZOOM_IN,
	ZOOM_OUT,
};

/* global variables */
static MU_32U g_frame_count = 0;
static muRect_t g_track_win;
static MU_32S g_stop_count = 0, g_pre_status = 0;
static MU_32S g_resolution_width=0, g_resolution_height=0;
static MU_32S g_tiltup_count=0, g_tiltdown_count=0;
static MU_32S g_pre_zoom_mode=0;
static MU_32S g_y_speed = INI_TILT_SPEED, g_x_speed=INI_PAN_SPEED;

//smooth direction x, y
static MU_32S g_centerx_count=0;
static MU_32S g_centery_count=0;
static muPoint_t g_pre_obj_position;

//variable speed control
static MU_32S g_is_zoom_in=0;
static MU_32S g_increase_spd=0;

/* the map of current object position */
/*  [0,0|0,1|0,2]  */
/*  |1,0|1,1|1,2|  */
/*  [2,0|2,1|2,2]  */
static muPoint_t g_obj_position;
static muImage_t *g_template_y = NULL;

//simulation motion dection
static muImage_t *g_pre_compare_img=NULL;
static muRect_t g_large_track_win;

//avoid ptz swing too many times
static MU_32S g_swing_count=0;
static MU_32S g_right_pos=0, g_left_pos=0;
static MU_32S g_up_pos=0, g_down_pos=0;
 

static void SavesubYuv(char x1[], int x2, int x3, FILE *x5, muImage_t *x6) 
{
	memset(x1, 0, 100);
	sprintf(x1 ,"sub422_%d_%d.yuv", x2, x3);
	x5 = fopen(x1, "wb");
	fwrite(x6->imagedata, 1, x2*x3*2, x5);
	fclose(x5);
}

static void SaveTemplateY(char x1[], int x2, int x3, FILE *x5, muImage_t *x6, int x7, int x8) 
{
	memset(x1, 0, 100);
	sprintf(x1 ,"TemplateY:mo%d_%dx%d_frame%d.yuv",x8, x2, x3,x7);
	x5 = fopen(x1, "wb");
	fwrite(x6->imagedata, 1, x2*x3, x5);
	fclose(x5);
}
static void SaveTemplateIni(char x1[], int x2, int x3, FILE *x5, muImage_t *x6, int x7) 
{
	memset(x1, 0, 100);
	sprintf(x1 ,"FirstY_%dx%d_frame%d.yuv", x2, x3,x7);
	x5 = fopen(x1, "wb");
	fwrite(x6->imagedata, 1, x2*x3, x5);
	fclose(x5);
}


/* Joe			*/
/* float -> int */
/* it should modify to independent input resolution */
/* return stop value */
static void Swing_verify(muPoint_t *direct_position)
{
	if(direct_position->x==2)
		g_right_pos=1;
	if(direct_position->x==0)
		g_left_pos=1;

	if(direct_position->y==2)
		g_down_pos=1;
	if(direct_position->y==0)
		g_up_pos=1;

	if((g_right_pos)&&(g_left_pos))
	{
		g_swing_count++;
		g_right_pos=0;
		g_left_pos=0;

	}

	if((g_down_pos)&&(g_up_pos))
	{
		g_swing_count++;
		g_down_pos=0;
		g_up_pos=0;
	}
		
	if(g_swing_count >= 4)
	{
		direct_position->x=1;
		direct_position->y=1;

		g_right_pos=0;
		g_left_pos=0;
		g_down_pos=0;
		g_up_pos=0;
	}
	
}
static MU_32S SetPtzAction(muRect_t trace_win, muPtzTrackingAction_t *action, muPoint_t *obj_position)
{
	MU_16S position_x[3];
	MU_16S position_y[3];
	MU_16S zone_width, zone_height;
	MU_16S win_center_x, win_center_y;
	MU_16S component_x, component_y;
	MU_16S chessboard_x, chessboard_y;
	MU_32S i,j,center_x, center_y;
	MU_32S distance, min = 0x7FFFFFFF;
	MU_32S status = 1;

	zone_width = g_resolution_width/3;
	zone_height = g_resolution_height/3;

	for(i=0; i<3; i++)
	{
		position_x[i] = i*zone_width;
		position_y[i] = i*zone_height;
	}
	
	win_center_x = (MU_16S)((2*trace_win.x + trace_win.width - 1)/2);
	win_center_y = (MU_16S)((2*trace_win.y + trace_win.height - 1)/2);

	chessboard_x = (MU_16S)(trace_win.x/zone_width);
	chessboard_y = (MU_16S)(trace_win.y/zone_height);

	for(i = -1; i <= 1; i++)
		for(j = -1; j <= 1; j++)
		{
			if( ((chessboard_x+i)<0)||((chessboard_x+i)>2)||((chessboard_y+j)<0)||((chessboard_y+j)>2) )
				continue;
			else
			{
				if(((chessboard_x+i)>2)||((chessboard_x+i)<0)||((chessboard_y+j)<0)||((chessboard_y+j)>2))
					printf("\n[MUGADGET]func_SetPtzAction error!!chessboard_x+i=%d, chessboard_y+j=%d\n",chessboard_x+i,chessboard_y+j);

				center_x = (MU_16S)((2*position_x[chessboard_x+i]+zone_width-1)/2);
				center_y = (MU_16S)((2*position_y[chessboard_y+j]+zone_height-1)/2);

				component_x = abs(center_x - win_center_x);
				component_x = component_x*component_x;

				component_y = abs(center_y - win_center_y);
				component_y = component_y*component_y;
	
				distance = (MU_32S)sqrt((float)(component_x + component_y));

				if(distance < min)
				{
					obj_position->x = chessboard_x + i;
					obj_position->y = chessboard_y + j;

					min = distance;
				}

			}
		}

	//smooth direction x 
	if(obj_position->x==1)
	{
		g_centerx_count++;
		if(g_centerx_count > NUMBERX)
			obj_position->x=1;
		else
			obj_position->x=g_pre_obj_position.x;
	}
	else
	{
		g_pre_obj_position.x = obj_position->x;
		g_centerx_count=0;
	}

	//smooth direction y
	if(obj_position->y==1)
	{
		g_centery_count++;
		if(g_centery_count > NUMBERY)
			obj_position->y=1;
		else
			obj_position->y = g_pre_obj_position.y;
	}
	else
	{
		g_pre_obj_position.y=obj_position->y;
		g_centery_count=0;
	}

	//variable spd control
	if(g_is_zoom_in)
	{
		g_increase_spd++;

		if(g_increase_spd > SPD_COUNT)
		{
			g_is_zoom_in=0;
			g_increase_spd=0;

			//g_y_speed /= 1.4;
			g_y_speed -= 5;
			g_y_speed = (g_y_speed < INI_TILT_SPEED)?INI_TILT_SPEED:g_y_speed;

		}
	}

	//avoid ptz swing too many times
	Swing_verify(obj_position);

	// control action
	switch(obj_position->x)
	{
		case 0://xdir -> left
			action->Xdir = 1;
			action->Xspd = 30;
		break;

		case 2:
			action->Xdir = 0;
			action->Xspd = 30;
		break;

		case 1:
			action->Xspd = 0;
		break;

		default:
			obj_position->x = 2;
			action->Xdir = 0;
			action->Xspd = 40;
			printf("\n[MU_APP] Ptz Cmd(map_x): XDir Error!!\n\n");
		break;
	}

	switch(obj_position->y)
	{
		case 0:
			action->Ydir = 0;
			action->Yspd = g_y_speed;//30//20
		break;

		case 2:
			action->Ydir = 1;
			action->Yspd = g_y_speed;//30//20
		break;

		case 1:
			action->Yspd = 0;
		break;

		default:
			obj_position->y = 2;
			action->Ydir = 1;
			action->Yspd = g_y_speed;
			printf("\n[MU_APP] Ptz Cmd(map_y): YDir Error %d!!\n\n", obj_position->y);
		break;
	}
	
	// obj in thre center --> obj didn't move.
	if(obj_position->y == 0)
		status = PTZ_TILT_UP;
	else if(obj_position->y == 2)
		status = PTZ_TILT_DOWN;
	else if(obj_position->y == 1 && obj_position->x == 1)
		status = PTZ_STOP;
	else
		status = PTZ_PAN;

	return status;
}

static void CleanPtzAction(muPtzTrackingAction_t* action)
{
    action->XValue = 0;
    action->YValue = 0;
    action->Operation = 0;

	action->Xspd = 0;
	action->Yspd = 0;
	
	g_obj_position.x = 1;
	g_obj_position.y = 1;

}

static muRect_t EnlargeTrackwin( muRect_t rect, const muImage_t *frame )//width x 1.5, height x 1.5
{
	float w_enlarge = ENLARGE_WIDTH;
	float back_minx=ENLARGE_MINX;
	float h_enlarge = ENLARGE_HEIGHT;
	float back_miny=ENLARGE_MINY;

	muRect_t dst_rect;

	if( (int)(rect.width *w_enlarge) > frame->width)//Rect.width*3/2* 2 * 3     
	{
		dst_rect.x = 0;

		dst_rect.width = frame->width;
	}
	else
	{
		dst_rect.x = rect.x - (int)( rect.width*back_minx);//Rect.width  /2 *3/4

		dst_rect.width = (int)(rect.width*w_enlarge);//Rect.width *3    *2 *5/2
		if(dst_rect.width%2 != 0)//Let the enlarged width be even.  
			dst_rect.width = dst_rect.width - 1;

		if(dst_rect.x < 0)
			dst_rect.x = 0;

		else if( (dst_rect.x + dst_rect.width) > frame->width )
			dst_rect.x = frame->width - dst_rect.width;

		if(dst_rect.x%2 != 0)
			dst_rect.x = dst_rect.x - 1;

	}

	if( (int)(rect.height *h_enlarge) > frame->height )//Rect.height *3  *2
	{
		dst_rect.y = 0;

		dst_rect.height = frame->height;
	}

	else
	{
		dst_rect.y = rect.y - (int)(rect.height*back_miny);//Rect.height  /2

		dst_rect.height = (int)(rect.height*h_enlarge);//Rect.height*2 *3 
		if(dst_rect.height%2 != 0)//Let the enlarged height be even.
			dst_rect.height = dst_rect.height -1;

		if(dst_rect.y < 0)
			dst_rect.y = 0;

		else if( (dst_rect.y + dst_rect.height) > frame->height )
			dst_rect.y = frame->height - dst_rect.height;

		if(dst_rect.y%2 != 0)
			dst_rect.y = dst_rect.y - 1;
	}

    return dst_rect;
}
static void GetMatchingTemplate(muPtzTargetInfo_t *info, muImage_t **template_y)//Get template from target_info
{
	MU_32S channel;
	MU_32S width, height;
	MU_32S sub_minx, sub_miny;	
	MU_32S sub_width, sub_height;
	muRect_t sub_rect;
	muImage_t *sub_img;

	width = info->yuv422->width;
	height = info->yuv422->height;
	channel = info->yuv422->channels;

	/* get sub yuv422 img */
	sub_minx = width/AIM_POSITION_X;
	sub_miny = height/AIM_POSITION_Y;

	if(sub_minx % 2 != 0)
		sub_minx++;
	if(sub_miny % 2 != 0)
		sub_miny++;
	
	sub_width = width/AIM_POSITION_W;
	sub_height = height/AIM_POSITION_H;

	if(sub_width % 2 != 0)
		sub_width++;
	if(sub_height % 2 != 0)
		sub_height++;

	sub_rect = muRect(sub_minx, sub_miny, sub_width, sub_height);
	sub_img = muCreateImage(muSize(sub_width, sub_height), MU_IMG_DEPTH_8U, channel);
	muSetZero(sub_img);

	if(channel == 3)
	muGetSubYUV422Image(info->yuv422, sub_img, sub_rect);
	else if(channel == 1)
	muGetSubImage(info->yuv422, sub_img, sub_rect);

	if(*template_y)
	muReleaseImage(&template_y);

	*template_y = muCreateImage(muSize(sub_width, sub_height), MU_IMG_DEPTH_8U, 1);
	memcpy((*template_y)->imagedata, sub_img->imagedata, sub_width*sub_height);

	muReleaseImage(&sub_img);

	return;
}

static double NCCTemplaeMatching(const muImage_t *cur_img, const muImage_t *cur_template, muImage_t *ncc_img, muRect_t *searchedwin)
{
	MU_32S n_h, n_w;
	MU_32S m_h, m_w;
	MU_32S t_w, t_h;
	MU_32S i, j, k, m;
	MU_32U sum=0, sum_i;
	MU_32S t_bar, f_bar;
	MU_32S dpixel;
	MU_32S f_prime, t_prime;
	MU_32S numerator;
	MU_32U f_dominator, t_dominator = 0;
	MU_64F dominator, r, maxr = -1.5, minr = 1.5;
	MU_32S minx, miny;
	MU_32S center_x, center_y;

	//masscenter
	MU_32S maxcen_x=0,maxcen_y=0;
	MU_32S cut_cenx,cut_ceny;
	MU_32S csw,csh,bufpixel,rowsum;
	MU_32S m00=0, m10=0, m01=0;
	MU_32S *colsum=NULL;
	MU_32S mass_x, mass_y, new_search_cenx, new_search_ceny;
	MU_32S tempminx, tempminy, newminx, newminy;
	//modify NCC 
	MU_64F *r_array=NULL;
	MU_64F tempscale;
	MU_32S corx, cory;

	MU_16S *cal_templ = NULL;
	MU_16S *diffTempl = NULL;
	muImage_t *cal_template=NULL;
	muImage_t *integral_img=NULL;
	MU_32U *data_temp;
	//masscenter
	muRect_t cut_rect;
	muImage_t *candidate_objset=NULL;

	//modifyNCC
	muImage_t *sub_ncc=NULL;

	n_h = cur_template->height; n_w = cur_template->width;
	m_h = cur_img->height; m_w = cur_img->width;
	t_w = n_w - 1;
	t_h = n_h - 1;

	//construct the gamma array
	csw = m_w-n_w+1; csh = m_h-n_h+1;
	r_array = (double *) calloc(csw*csh,sizeof(double));

	integral_img = muCreateImage(muSize(m_w, m_h), MU_IMG_DEPTH_32U, 1);
	muIntegralImage(cur_img, integral_img);

	//find t_bar
	for(i=0; i<n_h; i++)
		for(j=0; j<n_w; j++)
			sum += cur_template->imagedata[i*n_w+j];

	t_bar = sum/(n_w*n_h);
	if((t_bar>255)||(t_bar<0))
		printf("t_bar error!!,,t_bar=%d\n",t_bar);

	//find t(x-u, y-u)-t_bar
	cal_template = muCreateImage(muSize(n_w,n_h), MU_IMG_DEPTH_16S, 1);

	cal_templ = (MU_16S *) cal_template->imagedata;
	for(i=0; i<n_h; i++)
		for(j=0; j<n_w; j++)
		{
			dpixel = cur_template->imagedata[i*n_w+j] - t_bar;
			if((dpixel < -255)||(dpixel>255))
				printf("t_prime error!! num=%d,, idx =%d\n",dpixel,i*n_w+j);

			cal_templ[i*n_w+j] = dpixel;

			t_dominator += (dpixel*dpixel);

		}

	//find the main NCC procedure
	diffTempl = (MU_16S *)cal_template->imagedata;
	data_temp = (MU_32U *)integral_img->imagedata;

	for(i=0; i<(m_h-n_h+1); i++)
		for(j=0; j<(m_w-n_w+1); j++)
		{
			sum=0;
			//find f_bar in the template

			if(i == 0 && j == 0) 
			{
				sum_i = data_temp[t_w + m_w * t_h];
			}
			else if(i == 0 && j != 0)
			{
				sum_i = data_temp[j + t_w + m_w * t_h] - data_temp[j - 1 + m_w * t_h];
			}
			else if(i != 0 && j == 0)
			{
				sum_i = data_temp[t_w + m_w * (t_h + i)] - data_temp[t_w + m_w * (i - 1)];
			}
			else if(i !=0 && j != 0)
			{
				sum_i = data_temp[j + t_w + m_w * (t_h + i)] + data_temp[j - 1 + m_w * (i - 1)] -
					data_temp[j + t_w + m_w * (i - 1)] - data_temp[j - 1 + m_w * (t_h + i)];
			}

			f_bar = sum_i/(n_w*n_h);
			if((f_bar>255)||(f_bar<0))
				printf("f_bar error!!\n");

			//find numerator and dominaotr in each template
			numerator = 0; f_dominator=0;
			for(k=i; k<(i+n_h); k++)
				for(m=j; m<(j+n_w); m++)
				{
					//find Numerator
					f_prime = cur_img->imagedata[k*m_w+m] - f_bar;

					t_prime = diffTempl[(k-i)*n_w+(m-j)];

					numerator += (int)(f_prime*t_prime);

					//find Dominator
					f_dominator += (int)(f_prime*f_prime);
					if(f_dominator < 0)
						printf("error!!\n");
					//t_dominator += (t_prime*t_prime);

				}

			dominator = sqrt((long double)f_dominator) * sqrt((long double)t_dominator);

			//cal the gamma coef
			r = numerator/(double)dominator;

			if((r>1.0)||(r<-1.0))
			{
				printf("error!!\n");
				printf("r=%10f,,idx=%d\n",r,i*m_w+j);
			}

			center_x = (j+j+n_w-1)/2;
			center_y = (i+i+n_h-1)/2;
			
			//put gamma to r_array
			r_array[i*csw+j] = r;

			if(r>maxr)
			{
				maxr = r;
				minx = j;
				miny = i;
				maxcen_x = center_x;
				maxcen_y = center_y;
			}
			if(r<minr)
				minr = r;

		}// end for(j=0; j<(m_w-n_w-1); j++)

		sub_ncc = muCreateImage(muSize(csw,csh),MU_IMG_DEPTH_8U,1);
		muSetZero(sub_ncc);
		for(i=0; i<csw*csh; i++)
		{
			tempscale = (r_array[i]-minr)/(maxr-minr);
			if((tempscale>1.0)||(tempscale<0.))
				printf("errpr!!\n");
			sub_ncc->imagedata[i] = (unsigned char)(tempscale*255);
		}
		free(r_array);

		cory = (int)((n_h-1)/2); corx = (int)((n_w-1)/2);

		for(i=0; i<csh; i++)
			for(j=0; j<csw; j++)
				ncc_img->imagedata[(i+cory)*m_w+(j+corx)] = sub_ncc->imagedata[i*csw+j];

		muReleaseImage(&sub_ncc);


#if 1
	searchedwin->x = minx;
	searchedwin->y = miny;
	searchedwin->width = n_w;
	searchedwin->height = n_h;
#else
	//ready to decide the mass center of the objset

	/***find the rect in the cut center set***/
	cut_cenx = maxcen_x - (int)((n_w-1)/2);
	cut_ceny = maxcen_y - (int)((n_h-1)/2);

	cut_rect.x = cut_cenx-(csw/4);
	cut_rect.width = csw/2;
	if(cut_rect.x<0)
		cut_rect.x=0;
	else if(cut_rect.x+cut_rect.width > csw)
		cut_rect.x = csw - cut_rect.width;

	cut_rect.y = cut_ceny-(csh/4);
	cut_rect.height = csh/2;
	if(cut_rect.y<0)
		cut_rect.y = 0;
	else if(cut_rect.y+cut_rect.height > csh)
		cut_rect.y = csh - cut_rect.height;

	//put the cut_rect to real position in NCCImg
	cut_rect.x = cut_rect.x + (int)((n_w-1)/2);
	cut_rect.y = cut_rect.y + (int)((n_h-1)/2);
	/******************************************/

	/***find mass center***/
	candidate_objset = muCreateImage(muSize(cut_rect.width,cut_rect.height),MU_IMG_DEPTH_8U,1);
	muGetSubImage(ncc_img, candidate_objset, cut_rect);

	rowsum=0;
	m00=0;
	colsum = (int *) calloc(cut_rect.width,sizeof(int));
	for(i=0; i<cut_rect.height; i++)
	{
		for(j=0; j<cut_rect.width;j++)
		{
			bufpixel = candidate_objset->imagedata[i*cut_rect.width+j];

			m00 += bufpixel;
			rowsum += bufpixel;
			colsum[j] += bufpixel;
		}

		m10 += (i+1)*rowsum;
		rowsum=0;
	}

	for(i=0; i<cut_rect.width; i++)
		m01 += (i+1)*colsum[i];

	if(m00 == 0)
	{
		printf("\n[MUGADGET] NCCTemplateAPI m00 = 0!!\n\n");
		newminx = maxcen_x;
		newminx = maxcen_y;
	}
	else
	{
		mass_x = (int)(m01/(float)m00 + 0.5)-1;
		mass_y = (int)(m10/(float)m00 + 0.5)-1;

		new_search_cenx = mass_x + cut_rect.x;
		new_search_ceny = mass_y + cut_rect.y;
	}
	
	muReleaseImage(&candidate_objset);
	free(colsum);
	/**********************/

	/***map the center point to rcvtangle***/
	if(m00 != 0)
	{
		tempminx = (new_search_cenx*2)+1-n_w;
		newminx = (int)(tempminx/(float)2 + 0.5);

		tempminy = (new_search_ceny*2)+1-n_h;
		newminy = (int)(tempminy/(float)2+0.5);
	}
	/********************************/

	searchedwin->x = newminx;
	searchedwin->y = newminy;
	searchedwin->width = n_w;
	searchedwin->height = n_h;
#endif

	muReleaseImage(&cal_template);
	muReleaseImage(&integral_img);

	return maxr;

}
static void PtzZoomAction(muPtzTrackingAction_t *action, muPtzTrackingParas_t *paras, MU_32S zoommode)
{
	int current_zoom_ratio;

	switch(zoommode)
	{
		case ZOOM_IN:
			current_zoom_ratio = action->ZoomValue;

			action->ZoomValue = current_zoom_ratio*1.2;
			action->ZoomValue = (action->ZoomValue > paras->MaxZoomRatio)?paras->MaxZoomRatio:action->ZoomValue;

			//modify the speed after zoom in.
			g_y_speed += 10;
			g_y_speed = (g_y_speed > MAX_Y_SPD)?MAX_Y_SPD:g_y_speed;
			action->Yspd = g_y_speed;	
		break;

		case ZOOM_OUT:
			current_zoom_ratio = action->ZoomValue;

			action->ZoomValue = current_zoom_ratio/1.2;
			action->ZoomValue = (action->ZoomValue < MIN_ZOOM_RATIO)?MIN_ZOOM_RATIO:action->ZoomValue;

			//modify the speed after zoom out
			g_y_speed /= 1.3;
			g_y_speed = (g_y_speed < INI_TILT_SPEED)?INI_TILT_SPEED:g_y_speed;
			action->Yspd = g_y_speed;
		break;

		default:
			break;

	}

}
static MU_32S PtzZoomMode(MU_32S ret)
{
	int mode=CURRNET_ZOOM;

	switch(ret)
	{
		case PTZ_TILT_UP:
			if(!g_tiltdown_count)
				g_tiltup_count++;
			else
				g_tiltdown_count--;
		break;

		case PTZ_TILT_DOWN:
			if(!g_tiltup_count)
				g_tiltdown_count++;
			else
				g_tiltup_count--;
		break;

		default:
			break;
	}

	if((g_tiltup_count > 90)&&(g_tiltdown_count > 90))
		printf("\n[MUGADGET]Func: PtzZoomMode Error!!\n");

	if(g_tiltup_count > 90)//ptz zoom in
	{	
		g_tiltdown_count=0;
		g_tiltup_count=0;

		mode = ZOOM_IN;
	}

	if(g_tiltdown_count > 90)//ptz zoom out
	{
		g_tiltdown_count=0;
		g_tiltup_count=0;

		mode = ZOOM_OUT;
	}
		
	return mode;
}

static MU_32S VerifyLostObject(MU_32S frame_count, muRect_t large_track_win, muImage_t *copy_y_img)
{
	//DEEBUG
	FILE *saveptr;
	char name[100];

	MU_32S i;
	MU_32S temp_w,temp_h;
	MU_32S temp_pixel;
	MU_32S total_m_cnt=0;
	MU_32S stop_count=g_stop_count;
	muImage_t *cur_compare_img=NULL;
	muImage_t *diff_img=NULL;

	if(g_stop_count == 30)
	{
		if(g_pre_compare_img)
			printf("\n[MUGADGET]g_pre_compare_img control error!!\n");

		g_pre_compare_img = muCreateImage(muSize(large_track_win.width, large_track_win.height), MU_IMG_DEPTH_8U,1);

		muGetSubImage(copy_y_img,g_pre_compare_img,large_track_win);

		g_large_track_win = large_track_win;
		
		//DEBUG
		//SaveTemplateIni(name, g_large_track_win.width, g_large_track_win.height, saveptr, g_pre_compare_img, frame_count);

		stop_count = g_stop_count;

	}//end if(g_stop_count == 30)
	else
	{
		temp_w = g_large_track_win.width;
		temp_h = g_large_track_win.height;

		//find current compare image from the same g_large_track_win
		cur_compare_img = muCreateImage(muSize(temp_w,temp_h),MU_IMG_DEPTH_8U,1);
		muGetSubImage(copy_y_img,cur_compare_img,g_large_track_win);

		//find the diff image
		diff_img = muCreateImage(muSize(temp_w,temp_h),MU_IMG_DEPTH_8U,1);
		muSetZero(diff_img);

		total_m_cnt=0;

		for(i=0; i<temp_w*temp_h; i++)
		{
			temp_pixel = cur_compare_img->imagedata[i]-g_pre_compare_img->imagedata[i];

			temp_pixel = abs(temp_pixel);
			temp_pixel = (temp_pixel>FILTER_TH)?temp_pixel:0;

			if(temp_pixel)
				total_m_cnt++;

			diff_img->imagedata[i] = temp_pixel;
		}

		//DEBUG
		//SaveTemplateY(name, temp_w, temp_h, saveptr, diff_img, frame_count, total_m_cnt);

		//judge the motion
		if((total_m_cnt > 30)&&(total_m_cnt < 50))
		{
			stop_count = 29;

			if(g_pre_compare_img)
			{
				muReleaseImage(&g_pre_compare_img);
				g_pre_compare_img=NULL;
			}
			//printf("[MUGADGET] Keep tracking!!\n");
		}
		else
		{
			memcpy(g_pre_compare_img->imagedata,cur_compare_img->imagedata,temp_h*temp_w);

			stop_count = g_stop_count;
		}

		muReleaseImage(&cur_compare_img);
		muReleaseImage(&diff_img);

	}//end else    if(g_stop_count == 30)

	return stop_count;

}
MU_VOID muPtzTrackingInit(MU_32S width, MU_32S height)
{
	g_template_y = NULL;
	memset(&g_track_win, 0, sizeof(muRect_t));

	g_resolution_width = width;
	g_resolution_height = height;

	g_pre_compare_img=NULL;
}

MU_VOID muPtzTrackingReset()
{
	memset(&g_track_win, 0, sizeof(muRect_t));

	if(g_template_y)
	{
		muReleaseImage(&g_template_y);
		g_template_y = NULL;
	}

	g_pre_status = 0;
	g_stop_count = 0;
	g_frame_count = 0;

	g_tiltup_count=0;
	g_tiltdown_count=0;

	g_pre_zoom_mode=0;

	g_y_speed = INI_TILT_SPEED;

	//variable spd control
	g_is_zoom_in=0;
	g_increase_spd=0;

	//avoid ptz swing
	 g_pre_obj_position.x=1;
	 g_pre_obj_position.y=1;

	 //smooth control x,y
	 g_centerx_count=0;
	 g_centery_count=0;

	 //motion detection
	 if(g_pre_compare_img)
	 {
		 muReleaseImage(&g_pre_compare_img);
		 g_pre_compare_img=NULL;
	 }
	 
	 memset(&g_large_track_win, 0, sizeof(muRect_t));

	 //avoid ptz swing too many times
	 g_swing_count=0;
	 g_right_pos=0; 
	 g_left_pos=0;
	 g_up_pos=0; 
	 g_down_pos=0;

	return;
}

static MU_32S DrawBoundingBox(muImage_t *input, muRect_t rect)
{
	MU_32S width, height;
	MU_32S corx=0, cory=0, maxcorx=0, maxcory=0;

	width = input->width;
	height = input->height;
	
	//ready to draw the bounding box
	corx = rect.x;//show the Region position.
	cory = rect.y;
	maxcorx = corx + (rect.width - 1);
	maxcory = cory + (rect.height - 1);
	for(cory; cory <= maxcory; cory++)//region the object with white line in WholeBackproject and GrayImg.
	{         
		for(corx; corx <= maxcorx; corx++)
		{				
			if( (cory == rect.y) || (cory == maxcory) )
				input->imagedata[ cory * width + corx ] = 255;
			else
				if( (corx == rect.x) || (corx == maxcorx) )
					input->imagedata[ cory * width + corx ] = 255;

		}

		corx = rect.x;
	}
	return 0;
}

MU_32S muPtzTrackingTarget(const muImage_t *yuv422_img, muPtzTrackingParas_t *paras, muPtzTrackingAction_t *action)
{
	MU_32S track_target = 0;
	MU_32S width, height;
	MU_32S i,j;
	MU_32S win_w, win_h;
	MU_32S ret;
	MU_64F gamma;
	MU_32S mode=0;
	muRect_t rect, new_win;
	muRect_t large_track_win, sub_new_win;
	muImage_t *y_img = NULL;
	muImage_t *sub_img = NULL, *ncc_img = NULL;
	muPoint_t *obj_position;

	/*motion dection simulate*/
	muImage_t *copy_y_img=NULL;


	/* for debug */
	char name[100];

	g_frame_count++; 
	
	width = yuv422_img->width;
	height = yuv422_img->height;
	obj_position = (muPoint_t *)&g_obj_position;
	
	/* first time for tracking */
	/* (first) get template and initial tracking window */
	/* (after first) keep tracking */
	if(g_frame_count == 1)
	{
		GetMatchingTemplate(paras->info, &g_template_y);
		rect = paras->info->rect;
		g_track_win = muRect(rect.x, rect.y, rect.width, rect.height);
		SetPtzAction(g_track_win, action, obj_position);
		track_target = 1;
		return track_target;
	}
	else // continue tracking
	{
		/*****enlarge the tracking window*****/
		large_track_win = EnlargeTrackwin(g_track_win, yuv422_img);
		win_w = large_track_win.width; win_h = large_track_win.height;

		//make sure the EnLarge  function  is correct or not. 
		if((large_track_win.x<0)||(large_track_win.y<0)||(large_track_win.height<0)||(large_track_win.width<0))
		printf("\n[MUGADGET] large_track_win Error!!large_track_win.x=%d,,large_track_win.y=%d,,large_track_win.width=%d,,large_track_win.height=%d\n\n",large_track_win.x,large_track_win.y,large_track_win.width,large_track_win.height);

		if((large_track_win.x%2 != 0)||(large_track_win.y%2 != 0)||(large_track_win.height%2 != 0)||(large_track_win.width%2 != 0))
		printf("\n[MUGADGET] large_track_win even Error!!large_track_win.x=%d,,large_track_win.y=%d,,large_track_win.width=%d,,large_track_win.height=%d\n\n",large_track_win.x,large_track_win.y,large_track_win.width,large_track_win.height);
		/************************************/

		/***catch whole pYImg from the first width*height in pYUV422Img***/
		y_img = muCreateImage(muSize(width,height), MU_IMG_DEPTH_8U, 1);
		memcpy(y_img->imagedata, yuv422_img->imagedata, width*height);

		copy_y_img = muCreateImage(muSize(width,height), MU_IMG_DEPTH_8U, 1);
		muSetZero(copy_y_img);
		memcpy(copy_y_img->imagedata,y_img->imagedata,width*height);
		/********************************************************/

		/***Catch the subYImg from pYImg***/
		sub_img = muCreateImage(muSize(win_w,win_h), MU_IMG_DEPTH_8U, 1);
		muGetSubImage(y_img, sub_img, large_track_win);
		/**********************************/

		/***perform NCC Template Matching***/
		ncc_img = muCreateImage(muSize(win_w, win_h),MU_IMG_DEPTH_8U,1);
		muSetZero(ncc_img);

		gamma = NCCTemplaeMatching(sub_img, g_template_y, ncc_img, &sub_new_win);

		muReleaseImage(&sub_img);
		muReleaseImage(&ncc_img);

		new_win = muRect(sub_new_win.x+large_track_win.x, sub_new_win.y+large_track_win.y, sub_new_win.width, sub_new_win.height);

		// draw the bounding box
		DrawBoundingBox(y_img, new_win);
	
		CleanPtzAction(action);

		if(gamma > GAMMA_TH)
		{
			g_track_win = muRect(new_win.x, new_win.y, new_win.width, new_win.height);
			action->TrackWin = g_track_win;
			/***update the template_***/
			if(g_template_y != NULL)
			{
				muReleaseImage(&g_template_y);
				g_template_y = NULL;
			}

			g_template_y = muCreateImage(muSize(new_win.width,new_win.height),MU_IMG_DEPTH_8U,1);
			muSetZero(g_template_y);
			muGetSubImage(y_img, g_template_y, new_win);
			/***********************************/

			ret = SetPtzAction(g_track_win, action, obj_position);

			if(!ret) //object in the center
			{
				if(!g_pre_status)
				{
					g_stop_count++;
					
					//the motion judgement method
					if(g_stop_count >= 30)
						g_stop_count = VerifyLostObject(g_frame_count, large_track_win, copy_y_img);
					
				}
				else if(g_pre_status)
				{
					g_stop_count = 0;

					if(g_pre_compare_img)
					{
						muReleaseImage(&g_pre_compare_img);
						g_pre_compare_img=NULL;
					}
				}

				if(g_stop_count > STOP_FRAME)
				{
					if(g_pre_compare_img)
					{
						muReleaseImage(&g_pre_compare_img);
						g_pre_compare_img=NULL;
					}

					track_target = 0;
					g_stop_count = 0;
				}
				else
				{
					track_target = 1;
				}

				//avoid ptz swing too many times
				if(g_swing_count >= 4)
				{
					track_target = 0;
					g_swing_count=0;
					printf("[MUGADGET]PTZ swing too many times!! Lost!!\n");
				}
			}
			else
			{
				if(g_pre_zoom_mode != paras->ZoomMode)
				{
					g_tiltup_count=0;
					g_tiltdown_count=0;
				}

				if(paras->ZoomMode)//start perform zoom procedure
				{			 
					mode = PtzZoomMode(ret);

					switch(mode)
					{
						case ZOOM_IN:
							PtzZoomAction(action, paras, ZOOM_IN);
							g_is_zoom_in=1;
						break;

						case ZOOM_OUT:
							PtzZoomAction(action, paras, ZOOM_OUT);
						break;

						default:
							break;
					}
				}

				track_target = 1;
			}

			g_pre_status = ret;
			//printf("ret = %d  gamma = %f\n", ret ,gamma);
		}
		else
		{
			track_target = 0;
			printf("\n[MUGADGET] Lost Object!!   gamma = %f\n\n", gamma);
		}
		

		muReleaseImage(&copy_y_img);
		muReleaseImage(&y_img);
	}

	g_pre_zoom_mode = paras->ZoomMode;

	return track_target;
}

