/* ------------------------------------------------------------------------- /
 *
 * Module: muObjectdetector.c
 * Author: Joe Lin & Chao-Ting Hong
 * Create date: 11/03/2014
 *
 * Description:
 * This file is presented object detector with haar-cascade
 *  
 -------------------------------------------------------------------------- */
#include "muGadget.h"

/** Inline Functions */
__inline int ctRound(double dInput)
{
    if(dInput >= 0.0f)
    {
        return ((int)(dInput + 0.5f));
    }
    return ((int)(dInput - 0.5f));
}

typedef struct CtScaledRectPt
{
    int *p0, *p1, *p2, *p3;
} CtScaledRectPt;

static CtScaledRectPt EdgeP;
static int EdgeTH = 100;
static int EdgeRectTH;

void ctIntegral( const unsigned char* src, int* sum, double* sqsum, int* tilted, muSize_t size, int cn)
{
	int x, y, k;

    int srcstep = size.width;
    int sumstep = size.width+1;
    int tiltedstep = size.width+1;
    int sqsumstep = size.width+1;
	int t;
	double tq;
	int s;
    double sq;
	int* buf = NULL;

	cn = 1;
    size.width *= cn;

    memset( sum, 0, (size.width+cn)*sizeof(sum[0]));
    sum += sumstep + cn;

    if( sqsum )
    {
        memset( sqsum, 0, (size.width+cn)*sizeof(sqsum[0]));
        sqsum += sqsumstep + cn;
    }

    if( tilted )
    {
        memset( tilted, 0, (size.width+cn)*sizeof(tilted[0]));
        tilted += tiltedstep + cn;
    }

    if( sqsum == 0 && tilted == 0 )
    {
        for( y = 0; y < size.height; y++, src += srcstep - cn, sum += sumstep - cn )
        {
            for( k = 0; k < cn; k++, src++, sum++ )
            {
                int s = sum[-cn] = 0;
                for( x = 0; x < size.width; x += cn )
                {
                    s += src[x];
                    sum[x] = sum[x - sumstep] + s;
                }
            }
        }
    }
    else if( tilted == 0 )
    {
		for( y = 0; y < size.height; y++, src += srcstep - cn,
                        sum += sumstep - cn, sqsum += sqsumstep - cn )
        {
            for( k = 0; k < cn; k++, src++, sum++, sqsum++ )
            {
                int s = sum[-cn] = 0;
                double sq = sqsum[-cn] = 0;
                for( x = 0; x < size.width; x += cn )
                {
                    int it = src[x];
                    s += it;
                    sq += (double)it*it;
                    t = sum[x - sumstep] + s;
                    tq = sqsum[x - sqsumstep] + sq;
                    sum[x] = t;
                    sqsum[x] = tq;
                }
            }
        }
    }
    else
    {
		//AutoBuffer<int> _buf(size.width+cn);

		buf = (int *)calloc(size.width+cn, sizeof(int));

        for( k = 0; k < cn; k++, src++, sum++, tilted++, buf++ )
        {
            sum[-cn] = tilted[-cn] = 0;

            for( x = 0, s = 0, sq = 0; x < size.width; x += cn )
            {
                int it = src[x];
                buf[x] = tilted[x] = it;
                s += it;
                sq += (double)it*it;
                sum[x] = s;
                if( sqsum )
                    sqsum[x] = sq;
            }

            if( size.width == cn )
                buf[cn] = 0;

            if( sqsum )
            {
                sqsum[-cn] = 0;
                sqsum++;
            }
        }

        for( y = 1; y < size.height; y++ )
        {
            src += srcstep - cn;
            sum += sumstep - cn;
            tilted += tiltedstep - cn;
            buf += -cn;

            if( sqsum )
                sqsum += sqsumstep - cn;

            for( k = 0; k < cn; k++, src++, sum++, tilted++, buf++ )
            {
                int it = src[0];
                int t0 = s = it;
                double tq0 = sq = (double)it*it;

                sum[-cn] = 0;
                if( sqsum )
                    sqsum[-cn] = 0;
                tilted[-cn] = tilted[-tiltedstep];

                sum[0] = sum[-sumstep] + t0;
                if( sqsum )
                    sqsum[0] = sqsum[-sqsumstep] + tq0;
                tilted[0] = tilted[-tiltedstep] + t0 + buf[cn];

                for( x = cn; x < size.width - cn; x += cn )
                {
                    int t1 = buf[x];
                    buf[x - cn] = t1 + t0;
                    t0 = it = src[x];
                    tq0 = (double)it*it;
                    s += t0;
                    sq += tq0;
                    sum[x] = sum[x - sumstep] + s;
                    if( sqsum )
                        sqsum[x] = sqsum[x - sqsumstep] + sq;
                    t1 += buf[x + cn] + t0 + tilted[x - tiltedstep - cn];
                    tilted[x] = t1;
                }

                if( size.width > cn )
                {
                    int t1 = buf[x];
                    buf[x - cn] = t1 + t0;
                    t0 = it = src[x];
                    tq0 = (double)it*it;
                    s += t0;
                    sq += tq0;
                    sum[x] = sum[x - sumstep] + s;
                    if( sqsum )
                        sqsum[x] = sqsum[x - sqsumstep] + sq;
                    tilted[x] = t0 + t1 + tilted[x - tiltedstep - cn];
                    buf[x] = t0;
                }

                if( sqsum )
                    sqsum++;
            }
        }
		
		if(buf != NULL)
			free(buf);
    }
}

void muObjectDetectionInit( CtHaarClassifierCascade* cascade, CtHaarStageClassifier *cascade_stages, CtHaarClassifier *cascade_classifiers, double *CascadeParaTable)
{
    int i, j, k, l, rn;
    char chartemp[20];
    int Inttemp;
    double Flotemp;
    int has_tilted_features = 0;
    long index=0;

    //Original window size
    cascade->orig_window_size.width = CascadeParaTable[index];
    index++;
    cascade->orig_window_size.height = CascadeParaTable[index];
    index++;
    printf("Original window size: %d, %d\n",cascade->orig_window_size.width, cascade->orig_window_size.height);
    
    //Initial Stump and tree flag
    cascade->isStumpBased = 1;
    cascade->is_tree = 0;
    
    //Number of stages
    cascade->count = CascadeParaTable[index];
    index++;
    printf("Number of stages: %d\n",cascade->count);
    cascade->stage_classifier = cascade_stages; //mem asign
    //cascade->stage_classifier = new CtHaarStageClassifier[cascade->count];
    
    //Allocate mem for haar structures and read cascade parameter into haar structures
    for( i = 0; i < cascade->count; ++i ) //Read Stages
    {
        CtHaarStageClassifier *stage_classifier = (cascade->stage_classifier+i);
        stage_classifier->two_rects = 1;
        
        //Read number of trees in the i stage
        stage_classifier->count = CascadeParaTable[index];
        index++;
        printf("Number of classifiers in stage %d: %d\n", i, cascade->stage_classifier[i].count);
	
        //--Allocate mem for classifiers
        //stage_classifier->classifier = new CtHaarClassifier[stage_classifier->count]; 
        stage_classifier->classifier = cascade_classifiers;
        cascade_classifiers = cascade_classifiers + stage_classifier->count;
        
        for(j=0;j<stage_classifier->count;j++) //Read classifiers (weak)
        {
            CtHaarClassifier *classifier = stage_classifier->classifier+j;
            
            //--Read number of nodes in the j classifier
            classifier->count = CascadeParaTable[index];
            index++;
            #ifdef debug_load
            printf("Number of nodes in classifier %d: %d\n", j, cascade->stage_classifier[i].classifier[j].count);
            #endif
            //--Allocate mem for nodes			 
            cascade->isStumpBased &= classifier->count == 1;
            
            for(k=0;k<classifier->count;k++)
            {
            CtHaarTreeNode *node = &classifier->node;
            //--Read number of rects in the k node
            Inttemp = CascadeParaTable[index];
            index++;
            if(Inttemp==2) node->two_rects = 1;
            else if(Inttemp==3) node->two_rects = 0;
            else {printf("哇阿~~"); return;}
            #ifdef debug_load
            printf("If2Rect flag in node %d: %d\n", k, cascade->stage_classifier[i].classifier[j].node[k].two_rects);
            #endif
            if(node->two_rects)
            	rn = 2;
            else
            {
                rn = 3;
                stage_classifier->two_rects = 0;
                memset( &(node->feature.rect[2]), 0, sizeof(node->feature.rect[2]) );
            }
            for(l=0;l<rn;l++)
            {
            	node->feature.rect[l].r.x = CascadeParaTable[index];
                index++;
            	#ifdef debug_load
            	printf("x in rect %d: %d\n", l, cascade->stage_classifier[i].classifier[j].node[k].feature.rect[l].r.x);
            	#endif
            
            	node->feature.rect[l].r.y = CascadeParaTable[index];
                index++;
            	#ifdef debug_load
            	printf("y in rect %d: %d\n", l, cascade->stage_classifier[i].classifier[j].node[k].feature.rect[l].r.y);
            	#endif
            		 
            	node->feature.rect[l].r.width = CascadeParaTable[index];
                index++;
            	#ifdef debug_load
            	printf("width in rect %d: %d\n", l, cascade->stage_classifier[i].classifier[j].node[k].feature.rect[l].r.width);
            	#endif
            
            	node->feature.rect[l].r.height = CascadeParaTable[index];
                index++;
            	#ifdef debug_load
            	printf("height in rect %d: %d\n", l, cascade->stage_classifier[i].classifier[j].node[k].feature.rect[l].r.height);
            	#endif
            
            	node->feature.rect[l].ori_weight = CascadeParaTable[index];
                index++;
            	#ifdef debug_load
            	printf("weight in rect %d: %f\n", l, cascade->stage_classifier[i].classifier[j].node[k].feature.rect[l].weight);
            	#endif
            }
            	 
            node->feature.tilted = CascadeParaTable[index];
            index++;
            #ifdef debug_load
            printf("tilted flag in node %d: %d\n", k, cascade->stage_classifier[i].classifier[j].node[k].feature.tilted);
            #endif
            has_tilted_features |= node->feature.tilted != 0;
            
            //node->threshold = Flotemp;
            node->threshold = CascadeParaTable[index];
            index++;
            #ifdef debug_load
            printf("threshold in node %d: %f\n", k, cascade->stage_classifier[i].classifier[j].node[k].threshold);
            #endif
            node->left = CascadeParaTable[index];
            index++;
            #ifdef debug_load
            printf("left value in node %d: %f\n", k, cascade->stage_classifier[i].classifier[j].node[k].left);
            #endif
            node->right = CascadeParaTable[index];
            index++;
            #ifdef debug_load
            printf("right value in node %d: %f\n", k, cascade->stage_classifier[i].classifier[j].node[k].right);
            #endif
            } //end for k node
        }//end for j classifier
        
        stage_classifier->threshold = CascadeParaTable[index];
        index++;
        printf("threshold in stage %d: %f\n", i, cascade->stage_classifier[i].threshold);
        
        Inttemp = CascadeParaTable[index];
        index++;
        stage_classifier->parent = (Inttemp == -1) ? NULL : cascade->stage_classifier + Inttemp;
        
        Inttemp = CascadeParaTable[index];
        index++;
        stage_classifier->next = (Inttemp == -1) ? NULL : cascade->stage_classifier + Inttemp;
        stage_classifier->child = (Inttemp == -1) ? NULL : cascade->stage_classifier + i;
        cascade->is_tree |= stage_classifier->next != NULL;
        
    }

    cascade->has_tilted_features = has_tilted_features;
    
    printf("has tilted: %d, IsStump: %d, Istree: %d\n", cascade->has_tilted_features, cascade->isStumpBased, cascade->is_tree);
}

int ctRunHaarClassifierCascade_SuperLight( CtHaarClassifierCascade *cascade, muSize_t sumSize, int x, int y)
{
    int p_offset, pq_offset;
    int i, j;
    double mean, variance_norm_factor=-1;
    double stage_sum;
    
    pq_offset = p_offset = y * (sumSize.width) + x; //offset of sum
    //pq_offset = y * (sumSize.width) + x; //offset of sqsum
    mean = calc_sum(*cascade,p_offset)*cascade->inv_window_area;
    
    variance_norm_factor = cascade->pq0[pq_offset] - cascade->pq1[pq_offset] -
                           cascade->pq2[pq_offset] + cascade->pq3[pq_offset];
    variance_norm_factor = variance_norm_factor*cascade->inv_window_area - mean*mean; //Mean(Square sum) - mean(sum) square
    
    if( variance_norm_factor >= 0. )
        variance_norm_factor = sqrt(variance_norm_factor);
    else
        variance_norm_factor = 1.;

    if(variance_norm_factor<10)
        return 0;

    for( i = 0; i < cascade->count; i++ )
    {
        stage_sum = 0.0;
        
        if( cascade->stage_classifier[i].two_rects )
        {
            for( j = 0; j < cascade->stage_classifier[i].count; j++ )
            {
                CtHaarClassifier* classifier = cascade->stage_classifier[i].classifier + j;
                CtHaarTreeNode *node = &classifier->node; //Only one node~
                double t = node[0].threshold*variance_norm_factor;
                double sum1 = calc_sum(node[0].feature.rect[0],p_offset) * node[0].feature.rect[0].weight;
                sum1 += calc_sum(node[0].feature.rect[1],p_offset) * node[0].feature.rect[1].weight;
                stage_sum += sum1 >= t ? node[0].right:node[0].left;
            }
        }
        else
        {
            
            for( j = 0; j < cascade->stage_classifier[i].count; j++ )
            {
                CtHaarClassifier* classifier = cascade->stage_classifier[i].classifier + j;
                CtHaarTreeNode* node = &classifier->node;
                double t = node[0].threshold*variance_norm_factor;
                double sum1 = calc_sum(node[0].feature.rect[0],p_offset) * node[0].feature.rect[0].weight;
                sum1 += calc_sum(node[0].feature.rect[1],p_offset) * node[0].feature.rect[1].weight;
                
                if(!node[0].two_rects)
                    sum1 += calc_sum(node[0].feature.rect[2],p_offset) * node[0].feature.rect[2].weight;

                stage_sum += sum1 >= t ? node[0].right:node[0].left;
            }
        }

        if( stage_sum < cascade->stage_classifier[i].threshold )
        {
            return -i;
        }
    }

    return 1;
}


int ctRunHaarClassifierCascade( CtHaarClassifierCascade *cascade, muSize_t sumSize, int x, int y, int start_stage )
{
	int p_offset, pq_offset;
    int i, j;
    double mean, variance_norm_factor=-1;
	double stage_sum;
	
	if( x < 0 || y < 0 ||
        x + cascade->real_window_size.width >= sumSize.width ||
        y + cascade->real_window_size.height >= sumSize.height )
        return -1;

	pq_offset = p_offset = y * (sumSize.width) + x; //offset of sum
    //pq_offset = y * (sumSize.width) + x; //offset of sqsum
    mean = calc_sum(*cascade,p_offset)*cascade->inv_window_area; //
	
	variance_norm_factor = cascade->pq0[pq_offset] - cascade->pq1[pq_offset] -
                           cascade->pq2[pq_offset] + cascade->pq3[pq_offset];
    variance_norm_factor = variance_norm_factor*cascade->inv_window_area - mean*mean; //Mean(Square sum) - mean(sum) square
    if( variance_norm_factor >= 0. )
        variance_norm_factor = sqrt(variance_norm_factor);
    else
        variance_norm_factor = 1.;

	for( i = start_stage; i < cascade->count; i++ )
    {
        stage_sum = 0.0;
		
        if( cascade->stage_classifier[i].two_rects )
        {
            for( j = 0; j < cascade->stage_classifier[i].count; j++ )
            {
                CtHaarClassifier* classifier = cascade->stage_classifier[i].classifier + j;
                CtHaarTreeNode *node = &classifier->node; //Only one node~
                double t = node[0].threshold*variance_norm_factor;
                double sum1 = calc_sum(node[0].feature.rect[0],p_offset) * node[0].feature.rect[0].weight;
                sum1 += calc_sum(node[0].feature.rect[1],p_offset) * node[0].feature.rect[1].weight;
				stage_sum += sum1 >= t ? node[0].right:node[0].left;
            }
        }
        else
        {
            
			for( j = 0; j < cascade->stage_classifier[i].count; j++ )
            {
                CtHaarClassifier* classifier = cascade->stage_classifier[i].classifier + j;
                CtHaarTreeNode* node = &classifier->node;
                double t = node[0].threshold*variance_norm_factor;
                double sum1 = calc_sum(node[0].feature.rect[0],p_offset) * node[0].feature.rect[0].weight;
                sum1 += calc_sum(node[0].feature.rect[1],p_offset) * node[0].feature.rect[1].weight;
				
				if(!node[0].two_rects)
                    sum1 += calc_sum(node[0].feature.rect[2],p_offset) * node[0].feature.rect[2].weight;

                stage_sum += sum1 >= t ? node[0].right:node[0].left;
            }
        }

        if( stage_sum < cascade->stage_classifier[i].threshold )
		{
			return -i;
		}
    }

	return 1;
}


void ctSetImagesForHaarClassifierCascade( CtHaarClassifierCascade *cascade, muSize_t sumSize, int *sum, double *sqsum, int *tilted, int *EdgeSum, double scale )
{
	int i, index;
	int j, k, l;
	double weight_scale, win_area;
	CtRect equRect;

	if( cascade->has_tilted_features )
	{
		//cascade->tilted = *tilted;
	}
	//cascade->sum = *sum;
    //cascade->sqsum = *sqsum; //可能用不到
	
	cascade->scale = scale;
    cascade->real_window_size.width = ctRound( cascade->orig_window_size.width * scale );
    cascade->real_window_size.height = ctRound( cascade->orig_window_size.height * scale );

	//設定計算std的範圍:比real window (scane window)內縮round(scale) pixels
	equRect.x = equRect.y = ctRound(scale);
    equRect.width = ctRound((cascade->orig_window_size.width-2)*scale);
    equRect.height = ctRound((cascade->orig_window_size.height-2)*scale);
    win_area = (equRect.width*equRect.height);
    weight_scale = 1./(win_area);
    cascade->inv_window_area = weight_scale;
	//printf("scale %lf, equRect.x %d\n", scale, equRect.x);

	//設定計算std範圍的四點指到integral image上的指標 -> 考慮從sum實際有值的點(1,1)開始設定
	cascade->p0 = sum + sumSize.width*equRect.y + equRect.x;
	//cascade->p0 = sum + sumSize.width*1;
    cascade->p1 = sum + sumSize.width*equRect.y + equRect.x + equRect.width;
    cascade->p2 = sum + sumSize.width*(equRect.y + equRect.height) + equRect.x;
    cascade->p3 = sum + sumSize.width*(equRect.y + equRect.height) + equRect.x + equRect.width;

	cascade->pq0 = sqsum + sumSize.width*equRect.y + equRect.x;
    cascade->pq1 = sqsum + sumSize.width*equRect.y + equRect.x + equRect.width;
    cascade->pq2 = sqsum + sumSize.width*(equRect.y + equRect.height) + equRect.x;
    cascade->pq3 = sqsum + sumSize.width*(equRect.y + equRect.height) + equRect.x + equRect.width;

    if(EdgeSum!=0)
    {
        EdgeRectTH = EdgeTH*win_area;
        EdgeP.p0 = EdgeSum + equRect.y*sumSize.width + equRect.x;
        EdgeP.p1 = EdgeSum + equRect.y*sumSize.width + equRect.x + equRect.width;
        EdgeP.p2 = EdgeSum + (equRect.y + equRect.height)*sumSize.width + equRect.x;
        EdgeP.p3 = EdgeSum + (equRect.y + equRect.height)*sumSize.width + equRect.x + equRect.width;
    }

	for( i = 0; i < cascade->count; i++ )
    {
        for( j = 0; j < cascade->stage_classifier[i].count; j++ )
        {
            for( l = 0; l < cascade->stage_classifier[i].classifier[j].count; l++ )
            {
				CtHaarFeature* feature =
                    &cascade->stage_classifier[i].classifier[j].node.feature;
				double sum0 = 0, area0 = 0;
				CtRect r[3];

				int base_w = -1, base_h = -1;
                int new_base_w = 0, new_base_h = 0;
                int kx, ky;
                int flagx = 0, flagy = 0;
                int x0 = 0, y0 = 0;
                int nr;
				
				/* align blocks */
                for( k = 0; k < CT_HAAR_FEATURE_MAX; k++ )
                {
					if( cascade->stage_classifier[i].classifier[j].node.two_rects && k==2)
                        break;
                    r[k] = feature->rect[k].r; //feature's r to r, 原始座標及長寬
                    base_w = (int)CT_IMIN( (unsigned)base_w, (unsigned)(r[k].width-1) );
                    base_w = (int)CT_IMIN( (unsigned)base_w, (unsigned)(r[k].x - r[0].x-1) );
                    base_h = (int)CT_IMIN( (unsigned)base_h, (unsigned)(r[k].height-1) );
                    base_h = (int)CT_IMIN( (unsigned)base_h, (unsigned)(r[k].y - r[0].y-1) );
                }
				nr = cascade->stage_classifier[i].classifier[j].node.two_rects?2:3;
				base_w += 1;
                base_h += 1;
				if(base_w!=0)	//w
					kx = r[0].width / base_w;
				if(base_h!=0)	//w
                ky = r[0].height / base_h;

				if( kx <= 0 )
                {
                    flagx = 1;
					if(kx!=0)	//w
                    new_base_w = ctRound( r[0].width * scale ) / kx;
                    x0 = ctRound( r[0].x * scale );
                }

                if( ky <= 0 )
                {
                    flagy = 1;
					if(ky!=0)	//w
                    new_base_h = ctRound( r[0].height * scale ) / ky;
                    y0 = ctRound( r[0].y * scale );
                }

				for( k = 0; k < nr; k++ )  
                {
                    CtRect tr;
                    double correction_ratio;

                    if( flagx ) // r to tr
                    {
						if(base_w!=0)	//w
                        tr.x = (r[k].x - r[0].x) * new_base_w / base_w + x0;
						if(base_w!=0)	//w
                        tr.width = r[k].width * new_base_w / base_w;
                    }
                    else
                    {
                        tr.x = ctRound( r[k].x * scale );
                        tr.width = ctRound( r[k].width * scale );
                    }

                    if( flagy )
                    {
						if(base_h!=0)	//w
                        tr.y = (r[k].y - r[0].y) * new_base_h / base_h + y0;
						if(base_h!=0)	//w
                        tr.height = r[k].height * new_base_h / base_h;
                    }
                    else
                    {
                        tr.y = ctRound( r[k].y * scale );
                        tr.height = ctRound( r[k].height * scale );
                    }

#if MU_ADJUST_WEIGHTS
                    {
                    // RAINER START
                    const float orig_feature_size =  (float)(feature->rect[k].r.width)*feature->rect[k].r.height;
                    const float orig_norm_size = (float)(_cascade->orig_window_size.width)*(_cascade->orig_window_size.height);
                    const float feature_size = float(tr.width*tr.height);
                    //const float normSize    = float(equRect.width*equRect.height);
                    float target_ratio = orig_feature_size / orig_norm_size;
                    //float isRatio = featureSize / normSize;
                    //correctionRatio = targetRatio / isRatio / normSize;
                    correction_ratio = target_ratio / feature_size;
                    // RAINER END
                    }
#else
                    correction_ratio = weight_scale * (!feature->tilted ? 1 : 0.5);
					//correction_ratio = 1;
#endif

                    if( !feature->tilted )  //tr to hidfeature's r
                    {
                        feature->rect[k].p0 = sum + sumSize.width*tr.y + tr.x;
                        feature->rect[k].p1 = sum + sumSize.width*tr.y + tr.x + tr.width;
                        feature->rect[k].p2 = sum + sumSize.width*(tr.y + tr.height) + tr.x;
                        feature->rect[k].p3 = sum + sumSize.width*(tr.y + tr.height) + tr.x + tr.width;
                    }
                    /*
                    else
                    {
                        feature->rect[k].p2 = tilted + sumSize.width*(tr.y + tr.width) + tr.x + tr.width;
                        feature->rect[k].p3 = tilted + sumSize.width*(tr.y + tr.width + tr.height) + tr.x + tr.width - tr.height;
                        feature->rect[k].p0 = tilted + sumSize.width*tr.y, tr.x;
                        feature->rect[k].p1 = tilted + sumSize.width*(tr.y + tr.height) + tr.x - tr.height;
						//printf("YO");
                    }*/

                    feature->rect[k].weight = (float)(feature->rect[k].ori_weight * correction_ratio);

                    if( k == 0 )
                        area0 = tr.width * tr.height;
                    else
                        sum0 += feature->rect[k].weight * tr.width * tr.height;
                }

                feature->rect[0].weight = (float)(-sum0/area0);
			}
		}
	}
	//printf("test %d", base_w);
}

//Main tmp
void testtmp()
{
    ///Set examinator's sum, sqsum - locate mem for each video/resolution
     //muSize_t sumSize;
     //sumSize.width = img->width + 1;
     //sumSize.height = img->height + 1;
     //sum  = (int *)calloc(sumSize.width*sumSize.height, sizeof(int));
     //sqsum = (double *)calloc(sumSize.width*sumSize.height, sizeof(double));
    ///Calculate Integral image for each frame once

}

//Integral Image Light
//modify through muIntegralImage
//examinator->muIntegralImage Itlmg
//test
//opt #if win
//opt ctIntegral
//opt locate @ muexamin_run
muIntegralImg_t* muIntegral_Light(muImage_t *img)
{
    muIntegralImg_t *Itlmg;
    MU_8U *inputData;
    
    Itlmg = (muIntegralImg_t*)malloc(sizeof(muIntegralImg_t));
    Itlmg->sumSize.width = img->width + 1;
    Itlmg->sumSize.height = img->height + 1;
    Itlmg->sum  = (int *)malloc(Itlmg->sumSize.width*Itlmg->sumSize.height*sizeof(int));
    Itlmg->sqsum = (double *)malloc(Itlmg->sumSize.width*Itlmg->sumSize.height*sizeof(double));

    Itlmg->imgSize.width = img->width;
    Itlmg->imgSize.height = img->height;
    inputData = img->imagedata;
    ctIntegral(inputData, Itlmg->sum, Itlmg->sqsum, 0, Itlmg->imgSize, 1);
    return Itlmg;
}

void muIntegral_LightRelease(muIntegralImg_t* Itlmg)
{
    free(Itlmg->sum);
    free(Itlmg->sqsum);
}

//SetImage Light -- Wait for learning program done

//Object Detection Light
void muObjectDetection_Light(muIntegralImg_t *Itlmg, muRect_t ScanROI, muSeq_t* Objects, CtHaarClassifierCascade* cascade, double scaleFactor, muSize_t minSize, muSize_t maxSize)
{
    //Create result sequence
    muSize_t sumSize;
    int n_factors = 0;
    double factor;
    int iy, ix;
    int result, ixstep;
    int startX, startY;
    int endX, endY;

    ScanROI.x = ScanROI.x < 0 ? 0:ScanROI.x;
    ScanROI.y = ScanROI.y < 0 ? 0:ScanROI.y;
    ScanROI.x = ScanROI.x > Itlmg->imgSize.width ? Itlmg->imgSize.width:ScanROI.x;
    ScanROI.y = ScanROI.y > Itlmg->imgSize.height ? Itlmg->imgSize.height:ScanROI.y;

    ScanROI.width = (ScanROI.x+ScanROI.width) > Itlmg->imgSize.width ? Itlmg->imgSize.width-ScanROI.x:ScanROI.width;
    ScanROI.height = (ScanROI.y+ScanROI.height) > Itlmg->imgSize.height ? Itlmg->imgSize.height-ScanROI.y:ScanROI.height;

    for( n_factors = 0, factor = 1;
             factor*cascade->orig_window_size.width < ScanROI.width - 5 &&
             factor*cascade->orig_window_size.height < ScanROI.height - 5;
             n_factors++, factor *= scaleFactor );
    
    factor = 1;
    for( ; n_factors-- > 0; factor *= scaleFactor)
    {   
        const double ystep = factor > 2? factor: 2; //Scan step increase when window size increase after totalscalefactor is bigger than 2
        
        muSize_t winSize = { ctRound( cascade->orig_window_size.width * factor ),
                                ctRound( cascade->orig_window_size.height * factor )};
        
        CtRect rRect = { 0, 0, 0, 0 };

        startX = ScanROI.x;
        startY = ScanROI.y;
        endX = ScanROI.x+ScanROI.width - winSize.width;
        endY = ScanROI.y+ScanROI.height - winSize.height;

        if( winSize.width < minSize.width || winSize.height < minSize.height )
            continue;

        if ( winSize.width > maxSize.width || winSize.height > maxSize.height )
            break;

        ctSetImagesForHaarClassifierCascade( cascade, Itlmg->sumSize, Itlmg->sum, Itlmg->sqsum, 0, 0, factor );

        for( iy = startY; iy < endY; iy+=ystep )
        {
            ixstep = ystep;
            //int result;
            for( ix = startX; ix < endX; ix += ixstep )
            {
                result = ctRunHaarClassifierCascade( cascade, Itlmg->sumSize, ix, iy, 0 );
                if( result > 0 )
                {
                    rRect.x = ix;
                    rRect.y = iy;
                    rRect.width = winSize.width;
                    rRect.height = winSize.height;
                    //rectList.push_back(rRect);
                    muPushSeq(Objects, (MU_VOID *)&rRect);
                }
                ixstep = result != 0 ? ystep : ystep+1;
            }
        }
    }

    //free(sum);
    //free(sqsum);
}

//Object Detection Light
void muObjectDetection_SuperLight(muIntegralImg_t *Itlmg, muRect_t ScanROI, muSeq_t* Objects, CtHaarClassifierCascade* cascade, muSize_t winSize)
{
    //Create result sequence
    muSize_t sumSize;
    double factor, tmp_factor;
    int iy, ix;
    int result, ixstep;
    int startX, startY;
    int endX, endY;
    double ystep;
	CtRect rRect = { 0, 0, 0, 0 };

    ScanROI.x = ScanROI.x < 0 ? 0:ScanROI.x;
    ScanROI.y = ScanROI.y < 0 ? 0:ScanROI.y;
    ScanROI.x = ScanROI.x > Itlmg->imgSize.width ? Itlmg->imgSize.width:ScanROI.x;
    ScanROI.y = ScanROI.y > Itlmg->imgSize.height ? Itlmg->imgSize.height:ScanROI.y;

    ScanROI.width = (ScanROI.x+ScanROI.width) > Itlmg->imgSize.width ? Itlmg->imgSize.width-ScanROI.x:ScanROI.width;
    ScanROI.height = (ScanROI.y+ScanROI.height) > Itlmg->imgSize.height ? Itlmg->imgSize.height-ScanROI.y:ScanROI.height;

    //Scaling factor
    factor = winSize.width/cascade->orig_window_size.width;
    tmp_factor = winSize.height/cascade->orig_window_size.height;
    factor = factor > tmp_factor ? factor : tmp_factor;

    ystep = factor > 2? factor: 2; //Scan step increase when window size increase after totalscalefactor is bigger than 2
    
    winSize.width = ctRound( cascade->orig_window_size.width * factor );
    winSize.height = ctRound( cascade->orig_window_size.height * factor );

    startX = ScanROI.x;
    startY = ScanROI.y;
    endX = ScanROI.x+ScanROI.width - winSize.width;
    endY = ScanROI.y+ScanROI.height - winSize.height;

    ctSetImagesForHaarClassifierCascade( cascade, Itlmg->sumSize, Itlmg->sum, Itlmg->sqsum, 0, 0, factor );

    for( iy = startY; iy < endY; iy+=ystep )
    {
        ixstep = ystep;
        //int result;
        for( ix = startX; ix < endX; ix += ixstep )
        {
            result = ctRunHaarClassifierCascade_SuperLight( cascade, Itlmg->sumSize, ix, iy);
            if( result > 0 )
            {
                rRect.x = ix;
                rRect.y = iy;
                rRect.width = winSize.width;
                rRect.height = winSize.height;
                //rectList.push_back(rRect);
                muPushSeq(Objects, (MU_VOID *)&rRect);
            }
            ixstep = result != 0 ? ystep : ystep+1;
        }// for scanning x
    }// for scanning y 

}

void muObjectDetection1(muImage_t *img, muSeq_t* Objects, CtHaarClassifierCascade* cascade, double scaleFactor, int minNeighbors, int EdgeFlags, muSize_t minSize, muSize_t maxSize)
{
    //Create result sequence
    MU_8U *inputData;
    muSize_t sumSize;
    muSize_t imgSize;
    int *sum;
    double *sqsum;
    int *EdgeSum = NULL;
    //Pre calculate n_factors
    int n_factors = 0;
    double factor;
    //int *tilted = new int[sumSize.height*sumSize.width];
    int iy, ix;
    int x, y;
    int result, ixstep;

    sumSize.width = img->width + 1;
    sumSize.height = img->height + 1;
    imgSize.width = img->width;
    imgSize.height = img->height;
    inputData = img->imagedata;
    sum  = (int *)calloc(sumSize.width*sumSize.height, sizeof(int));
    sqsum = (double *)calloc(sumSize.width*sumSize.height, sizeof(double));

    //if(cascade->has_tilted_features)
    //    ctIntegral(img,sum,sqsum,tilted,imgSize,1);
    //else
    ctIntegral(inputData, sum, sqsum, 0, imgSize, 1);

    if(EdgeFlags==1)
    {
        unsigned char *EdgeImg;
        
        EdgeImg = (unsigned char *)calloc(imgSize.height*imgSize.width, sizeof(unsigned char));
        
        EdgeSum = (int *)calloc(sumSize.height*sumSize.width, sizeof(int));
    
        //prewitt
        //SobelFilter(img, EdgeImg, imgSize.width, imgSize.height);
        ctIntegral(EdgeImg,EdgeSum,0,0,imgSize,1);
        free(EdgeImg);
    }


    for( n_factors = 0, factor = 1;
             factor*cascade->orig_window_size.width < imgSize.width - 10 &&
             factor*cascade->orig_window_size.height < imgSize.height - 10;
             n_factors++, factor *= scaleFactor );
    
    factor = 1;
    for( ; n_factors-- > 0; factor *= scaleFactor)
    {   
        const double ystep = factor > 2? factor: 2; //Scan step increase when window size increase after totalscalefactor is bigger than 2
        
        muSize_t winSize = { ctRound( cascade->orig_window_size.width * factor ),
                                ctRound( cascade->orig_window_size.height * factor )};
        
        CtRect rRect = { 0, 0, 0, 0 };
        int startX = 0, startY = 0;
        int endX = ctRound((imgSize.width - winSize.width) / ystep);
        int endY = ctRound((imgSize.height - winSize.height) / ystep);

        if( winSize.width < minSize.width || winSize.height < minSize.height )
            continue;

        if ( winSize.width > maxSize.width || winSize.height > maxSize.height )
            break;

        ctSetImagesForHaarClassifierCascade( cascade, sumSize, sum, sqsum, 0, EdgeSum, factor );

        for( iy = startY; iy < endY; iy++ )
        {
            y = ctRound(iy*ystep);
            ixstep = 1;
            //int result;
            for( ix = startX; ix < endX; ix += ixstep )
            {
                if( EdgeFlags==1 )
                {
                    int p_offset = iy*imgSize.width + ix;
                    int EdgeS = calc_sum(EdgeP, p_offset);
                    if( EdgeS < EdgeRectTH )
                    {
                        ixstep = 2;
                        continue;
                    }
                }
                x = ctRound(ix*ystep); // it should really be ystep, not ixstep
                result = ctRunHaarClassifierCascade( cascade, sumSize, x, y, 0 );
                if( result > 0 )
                {
                    rRect.x = x;
                    rRect.y = y;
                    rRect.width = winSize.width;
                    rRect.height = winSize.height;
                    //rectList.push_back(rRect);
                    muPushSeq(Objects, (MU_VOID *)&rRect);
                }
                ixstep = result != 0 ? 1 : 2;
            }
        }
    }
    
    free(sum);
    free(sqsum);
    if(EdgeSum)
    free(EdgeSum);
    //delete tilted;
}

muSeq_t *muObjectDetection(muImage_t *img, CtHaarClassifierCascade* cascade, double scaleFactor, int minNeighbors, int EdgeFlags, muSize_t minSize, muSize_t maxSize)
{
    //Create result sequence
	MU_8U *inputData;
	muSeq_t *rectList;
	muSize_t sumSize;
	muSize_t imgSize;
	int *sum;
	double *sqsum;
    int *EdgeSum = NULL;
	//Pre calculate n_factors
	int n_factors = 0;
	double factor;
	//int *tilted = new int[sumSize.height*sumSize.width];
	int iy, ix;
	int x, y;
	int result, ixstep;

	sumSize.width = img->width + 1;
	sumSize.height = img->height + 1;
	imgSize.width = img->width;
	imgSize.height = img->height;
	inputData = img->imagedata;
	sum  = (int *)calloc(sumSize.width*sumSize.height, sizeof(int));
	sqsum = (double *)calloc(sumSize.width*sumSize.height, sizeof(double));

	rectList = muCreateSeq(sizeof(CtRect));
	//if(cascade->has_tilted_features)
    //    ctIntegral(img,sum,sqsum,tilted,imgSize,1);
	//else
	ctIntegral(inputData, sum, sqsum, 0, imgSize, 1);

    if(EdgeFlags==1)
    {
        unsigned char *EdgeImg;
		
		EdgeImg = (unsigned char *)calloc(imgSize.height*imgSize.width, sizeof(unsigned char));
		
        EdgeSum = (int *)calloc(sumSize.height*sumSize.width, sizeof(int));
	
		//prewitt
        //SobelFilter(img, EdgeImg, imgSize.width, imgSize.height);
        ctIntegral(EdgeImg,EdgeSum,0,0,imgSize,1);
		free(EdgeImg);
    }


	for( n_factors = 0, factor = 1;
             factor*cascade->orig_window_size.width < imgSize.width - 10 &&
			 factor*cascade->orig_window_size.height < imgSize.height - 10;
             n_factors++, factor *= scaleFactor );
	
	factor = 1;
	for( ; n_factors-- > 0; factor *= scaleFactor)
    {	
		const double ystep = factor > 2? factor: 2; //Scan step increase when window size increase after totalscalefactor is bigger than 2
		
		muSize_t winSize = { ctRound( cascade->orig_window_size.width * factor ),
                                ctRound( cascade->orig_window_size.height * factor )};
		
		CtRect rRect = { 0, 0, 0, 0 };
        int startX = 0, startY = 0;
		int endX = ctRound((imgSize.width - winSize.width) / ystep);
		int endY = ctRound((imgSize.height - winSize.height) / ystep);

        if( winSize.width < minSize.width || winSize.height < minSize.height )
            continue;

        if ( winSize.width > maxSize.width || winSize.height > maxSize.height )
            break;

		ctSetImagesForHaarClassifierCascade( cascade, sumSize, sum, sqsum, 0, EdgeSum, factor );

        for( iy = startY; iy < endY; iy++ )
        {
            y = ctRound(iy*ystep);
			ixstep = 1;
			//int result;
            for( ix = startX; ix < endX; ix += ixstep )
            {
                if( EdgeFlags==1 )
                {
                    int p_offset = iy*imgSize.width + ix;
                    int EdgeS = calc_sum(EdgeP, p_offset);
                    if( EdgeS < EdgeRectTH )
                    {
                        ixstep = 2;
                        continue;
                    }
                }
                x = ctRound(ix*ystep); // it should really be ystep, not ixstep
                result = ctRunHaarClassifierCascade( cascade, sumSize, x, y, 0 );
                if( result > 0 )
				{
				    rRect.x = x;
				    rRect.y = y;
				    rRect.width = winSize.width;
				    rRect.height = winSize.height;
				    //rectList.push_back(rRect);
					muPushSeq(rectList, (MU_VOID *)&rRect);
				}
				ixstep = result != 0 ? 1 : 2;
            }
        }
	}
	
	free(sum);
	free(sqsum);
	if(EdgeSum)
	free(EdgeSum);
	//delete tilted;

    return rectList;
}


