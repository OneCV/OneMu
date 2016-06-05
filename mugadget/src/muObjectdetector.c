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
#define MU_ADJUST_WEIGHTS 0

void muCalcIntegralImage( const unsigned char* src, int* sum, double* sqsum, muSize_t size)
{
	int x, y;

    int srcstep = size.width;
    int sumstep = size.width+1;
    int sqsumstep = size.width+1;
	int t;
	double tq;
	int s;
    double sq;

    memset( sum, 0, (size.width+1)*sizeof(sum[0]));
    sum+=sumstep + 1;

    if( sqsum )
    {
        memset( sqsum, 0, (size.width+1)*sizeof(sqsum[0]));
        sqsum+=sqsumstep+1;
    }

    if( sqsum==0 )
    {
        for( y=0; y<size.height; y++, src+=srcstep, sum+=sumstep )
        {
            int s = sum[-1] = 0;
            for( x=0; x<size.width; x++ )
            {
                s+=src[x];
                sum[x]=sum[x-sumstep]+s;
            }
        }
    }
    else
    {
		for( y=0; y<size.height; y++, src += srcstep, sum += sumstep, sqsum += sqsumstep)
        {
            int s = sum[-1] = 0;
            double sq = sqsum[-1] = 0;
            for( x = 0; x < size.width; x++ )
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


 MuSimpleDetector* muLoadSimpleDetector( const char* filename)
 {
     MuSimpleDetector *cascade = (MuSimpleDetector *)calloc(1, sizeof(MuSimpleDetector));

     FILE *cFileP = NULL;
     int i, j, k, l, rn;
     char chartemp[20];
     int Inttemp;
     double Flotemp;
     int has_tilted_features = 0;

     printf("\nRead Cascacade File\n");
     cFileP = fopen(filename, "r");
     if(cFileP == NULL)
     {
         printf("Open file error XDrz\n");
         return 0;
     }
     
     //Original window size
     fscanf(cFileP, "%d", &Inttemp);
     cascade->orig_window_size.width = Inttemp;
     fscanf(cFileP, "%d", &Inttemp);
     cascade->orig_window_size.height = Inttemp;
     printf("Original window size: %d, %d\n",cascade->orig_window_size.width, cascade->orig_window_size.height);

     //Initial Stump and tree flag
     cascade->isStumpBased = 1;
     cascade->is_tree = 0;

     //Number of stages
     fscanf(cFileP, "%d", &Inttemp);
     cascade->count = Inttemp;
     printf("Number of stages: %d\n",cascade->count);
     cascade->stage_classifier = (MuHaarStageClassifier *)calloc(cascade->count, sizeof(MuHaarStageClassifier));

     //Allocate mem for haar structures and read cascade parameter into haar structures
     for( i = 0; i < cascade->count; ++i ) //Read Stages
     {
         MuHaarStageClassifier *stage_classifier = (cascade->stage_classifier+i);        
         
         stage_classifier->two_rects = 1;

         //Read number of trees in the i stage
         fscanf(cFileP, "%d", &Inttemp);  
         stage_classifier->count = Inttemp;
         printf("Number of classifiers in stage %d: %d\n", i, cascade->stage_classifier[i].count);
         
         //--Allocate mem for classifiers
         stage_classifier->classifier = (MuHaarClassifier *)calloc(stage_classifier->count, sizeof(MuHaarClassifier));
         for(j=0;j<stage_classifier->count;j++) //Read classifiers (weak)
         {
             MuHaarClassifier *classifier = stage_classifier->classifier+j;

             //--Read number of nodes in the j classifier
             fscanf(cFileP, "%d", &Inttemp);
             classifier->count = Inttemp;
             
             #ifdef debug_load
             printf("Number of nodes in classifier %d: %d\n", j, cascade->stage_classifier[i].classifier[j].count);
             #endif

             cascade->isStumpBased &= classifier->count == 1;

             for(k=0;k<classifier->count;k++)
             {
                MuHaarTreeNode *node = &classifier->node;
                //--Read number of rects in the k node
                fscanf(cFileP, "%d", &Inttemp);
                if(Inttemp==2) node->two_rects = 1;
                else if(Inttemp==3) node->two_rects = 0;
                else {printf("哇阿~~"); return 0;}
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
                    fscanf(cFileP, "%d", &Inttemp);
                    node->feature.rect[l].r.x = Inttemp;
                    #ifdef debug_load
                    printf("x in rect %d: %d\n", l, cascade->stage_classifier[i].classifier[j].node[k].feature.rect[l].r.x);
                    #endif

                    fscanf(cFileP, "%d", &Inttemp);
                    node->feature.rect[l].r.y = Inttemp;
                    #ifdef debug_load
                    printf("y in rect %d: %d\n", l, cascade->stage_classifier[i].classifier[j].node[k].feature.rect[l].r.y);
                    #endif
                         
                    fscanf(cFileP, "%d", &Inttemp);
                    node->feature.rect[l].r.width = Inttemp;
                    #ifdef debug_load
                    printf("width in rect %d: %d\n", l, cascade->stage_classifier[i].classifier[j].node[k].feature.rect[l].r.width);
                    #endif

                    fscanf(cFileP, "%d", &Inttemp);
                    node->feature.rect[l].r.height = Inttemp;
                    #ifdef debug_load
                    printf("height in rect %d: %d\n", l, cascade->stage_classifier[i].classifier[j].node[k].feature.rect[l].r.height);
                    #endif

                    fscanf(cFileP, "%lf", &Flotemp);
                    node->feature.rect[l].ori_weight = Flotemp;
                    #ifdef debug_load
                    printf("weight in rect %d: %f\n", l, cascade->stage_classifier[i].classifier[j].node[k].feature.rect[l].weight);
                    #endif
                }
                     
                fscanf(cFileP, "%d", &Inttemp);
                node->feature.tilted=Inttemp;
                #ifdef debug_load
                printf("tilted flag in node %d: %d\n", k, cascade->stage_classifier[i].classifier[j].node[k].feature.tilted);
                #endif
                has_tilted_features |= node->feature.tilted != 0;

                fscanf(cFileP, "%lf", &Flotemp);
                node->threshold = Flotemp;
                #ifdef debug_load
                printf("threshold in node %d: %f\n", k, cascade->stage_classifier[i].classifier[j].node[k].threshold);
                #endif
                fscanf(cFileP, "%lf", &Flotemp);
                node->left = Flotemp;
                #ifdef debug_load
                printf("left value in node %d: %f\n", k, cascade->stage_classifier[i].classifier[j].node[k].left);
                #endif
                fscanf(cFileP, "%lf", &Flotemp);
                node->right = Flotemp;
                #ifdef debug_load
                printf("right value in node %d: %f\n", k, cascade->stage_classifier[i].classifier[j].node[k].right);
                #endif

                //ignore tilted feature
                if(node->feature.tilted!=0){
                    node->left = 0;
                    node->right = 0;
                }
             } //end for k node
         }//end for j classifier

         fscanf(cFileP, "%lf", &Flotemp);
         stage_classifier->threshold = Flotemp;
         printf("threshold in stage %d: %f\n", i, cascade->stage_classifier[i].threshold);

         fscanf(cFileP, "%d", &Inttemp);
         stage_classifier->parent = (Inttemp == -1) ? NULL : cascade->stage_classifier + Inttemp;

         fscanf(cFileP, "%d", &Inttemp);
         stage_classifier->next = (Inttemp == -1) ? NULL : cascade->stage_classifier + Inttemp;
         stage_classifier->child = (Inttemp == -1) ? NULL : cascade->stage_classifier + i;
         cascade->is_tree |= stage_classifier->next != NULL;
     }

     cascade->has_tilted_features = has_tilted_features;
     
     printf("has tilted: %d, IsStump: %d, Istree: %d\n", cascade->has_tilted_features, cascade->isStumpBased, cascade->is_tree);

     fclose(cFileP);
     return cascade;
 }

void muObjectDetectionInit( MuSimpleDetector* cascade, MuHaarStageClassifier *cascade_stages, MuHaarClassifier *cascade_classifiers, double *CascadeParaTable)
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
    
    //Allocate mem for haar structures and read cascade parameter into haar structures
    for( i = 0; i < cascade->count; ++i ) //Read Stages
    {
        MuHaarStageClassifier *stage_classifier = (cascade->stage_classifier+i);
        stage_classifier->two_rects = 1;
        
        //Read number of trees in the i stage
        stage_classifier->count = CascadeParaTable[index];
        index++;
        printf("Number of classifiers in stage %d: %d\n", i, cascade->stage_classifier[i].count);
	
        //--Allocate mem for classifiers
        stage_classifier->classifier = cascade_classifiers;
        cascade_classifiers = cascade_classifiers + stage_classifier->count;
        
        for(j=0;j<stage_classifier->count;j++) //Read classifiers (weak)
        {
            MuHaarClassifier *classifier = stage_classifier->classifier+j;
            
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
            MuHaarTreeNode *node = &classifier->node;
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

void muReleaseSimpleDetector( MuSimpleDetector* cascade )
{
    int i, j;

    //Delete mem for cascade structures
    for( i = 0; i < cascade->count; ++i ) //Read Stages
    {
        MuHaarStageClassifier *stage_classifier = (cascade->stage_classifier+i);
        free(stage_classifier->classifier);
    }

    free(cascade->stage_classifier);
    free(cascade);
}

int ctRunHaarClassifierCascade_SuperLight( MuSimpleDetector *cascade, muSize_t sumSize, int x, int y)
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
                MuHaarClassifier* classifier = cascade->stage_classifier[i].classifier + j;
                MuHaarTreeNode *node = &classifier->node; //Only one node~
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
                MuHaarClassifier* classifier = cascade->stage_classifier[i].classifier + j;
                MuHaarTreeNode* node = &classifier->node;
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


int ctRunHaarClassifierCascade( MuSimpleDetector *cascade, muSize_t sumSize, int x, int y, int std_th )
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
    mean = calc_sum(*cascade,p_offset)*cascade->inv_window_area;
	
	variance_norm_factor = cascade->pq0[pq_offset] - cascade->pq1[pq_offset] -
                           cascade->pq2[pq_offset] + cascade->pq3[pq_offset];
    variance_norm_factor = variance_norm_factor*cascade->inv_window_area - mean*mean; //Mean(Square sum) - mean(sum) square
    if( variance_norm_factor >= 0. )
        variance_norm_factor = sqrt(variance_norm_factor);
    else
        variance_norm_factor = 1.;

    if(variance_norm_factor < std_th)
        return 0;

	for( i = 0; i < cascade->count; i++ )
    {
        stage_sum = 0.0;
		
        if( cascade->stage_classifier[i].two_rects )
        {
            for( j = 0; j < cascade->stage_classifier[i].count; j++ )
            {
                MuHaarClassifier* classifier = cascade->stage_classifier[i].classifier + j;
                MuHaarTreeNode *node = &classifier->node; //Only one node~
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
                MuHaarClassifier* classifier = cascade->stage_classifier[i].classifier + j;
                MuHaarTreeNode* node = &classifier->node;
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


void muSetImagesForHaarClassifierCascade( MuSimpleDetector *cascade, muSize_t sumSize, int *sum, double *sqsum, double scale )
{
	int i, j, k, l;
	double weight_scale, win_area;
	muRect_t equRect;

	cascade->scale = scale;
    cascade->real_window_size.width = muRound( cascade->orig_window_size.width * scale );
    cascade->real_window_size.height = muRound( cascade->orig_window_size.height * scale );

	//Set rectangle area for weight scaling and std calculation
	equRect.x = equRect.y = muRound(scale);
    equRect.width = muRound((cascade->orig_window_size.width-2)*scale);
    equRect.height = muRound((cascade->orig_window_size.height-2)*scale);
    win_area = (equRect.width*equRect.height);
    weight_scale = 1./(win_area);
    cascade->inv_window_area = weight_scale;

	//Set pointers for std calculation
	cascade->p0 = sum + sumSize.width*equRect.y + equRect.x;
    cascade->p1 = sum + sumSize.width*equRect.y + equRect.x + equRect.width;
    cascade->p2 = sum + sumSize.width*(equRect.y + equRect.height) + equRect.x;
    cascade->p3 = sum + sumSize.width*(equRect.y + equRect.height) + equRect.x + equRect.width;

	cascade->pq0 = sqsum + sumSize.width*equRect.y + equRect.x;
    cascade->pq1 = sqsum + sumSize.width*equRect.y + equRect.x + equRect.width;
    cascade->pq2 = sqsum + sumSize.width*(equRect.y + equRect.height) + equRect.x;
    cascade->pq3 = sqsum + sumSize.width*(equRect.y + equRect.height) + equRect.x + equRect.width;

	for( i = 0; i < cascade->count; i++ )
    {
        for( j = 0; j < cascade->stage_classifier[i].count; j++ )
        {
            for( l = 0; l < cascade->stage_classifier[i].classifier[j].count; l++ )
            {
				MuHaarFeature* feature = &cascade->stage_classifier[i].classifier[j].node.feature;
				double sum0 = 0, area0 = 0;
				muRect_t r[3];

				int base_w = -1, base_h = -1;
                int new_base_w = 0, new_base_h = 0;
                int kx, ky;
                int flagx = 0, flagy = 0;
                int x0 = 0, y0 = 0;
                int nr;
				
				/* align blocks */
                for( k = 0; k < MU_HAAR_FEATURE_MAX; k++ )
                {
					if( cascade->stage_classifier[i].classifier[j].node.two_rects && k==2)
                        break;
                    r[k] = feature->rect[k].r; //assign feature's r to r
                    base_w = (int)MU_IMIN( (unsigned)base_w, (unsigned)(r[k].width-1) );
                    base_w = (int)MU_IMIN( (unsigned)base_w, (unsigned)(r[k].x - r[0].x-1) );
                    base_h = (int)MU_IMIN( (unsigned)base_h, (unsigned)(r[k].height-1) );
                    base_h = (int)MU_IMIN( (unsigned)base_h, (unsigned)(r[k].y - r[0].y-1) );
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
                    new_base_w = muRound( r[0].width * scale ) / kx;
                    x0 = muRound( r[0].x * scale );
                }

                if( ky <= 0 )
                {
                    flagy = 1;
					if(ky!=0)	//w
                    new_base_h = muRound( r[0].height * scale ) / ky;
                    y0 = muRound( r[0].y * scale );
                }

				for( k = 0; k < nr; k++ )  
                {
                    muRect_t tr;
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
                        tr.x = muRound( r[k].x * scale );
                        tr.width = muRound( r[k].width * scale );
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
                        tr.y = muRound( r[k].y * scale );
                        tr.height = muRound( r[k].height * scale );
                    }

#if MU_ADJUST_WEIGHTS
                    {
                    // RAINER START
                    const float orig_feature_size = (float)(feature->rect[k].r.width)*feature->rect[k].r.height;
                    const float orig_norm_size = (float)(cascade->orig_window_size.width)*(cascade->orig_window_size.height);
                    const float feature_size = (float)(tr.width*tr.height);
                    float target_ratio = orig_feature_size / orig_norm_size;
                    correction_ratio = target_ratio / feature_size;
                    // RAINER END
                    }
#else
                    correction_ratio = weight_scale;
#endif

                    if( !feature->tilted )  //tr to hidfeature's r
                    {
                        feature->rect[k].p0 = sum + sumSize.width*tr.y + tr.x;
                        feature->rect[k].p1 = sum + sumSize.width*tr.y + tr.x + tr.width;
                        feature->rect[k].p2 = sum + sumSize.width*(tr.y + tr.height) + tr.x;
                        feature->rect[k].p3 = sum + sumSize.width*(tr.y + tr.height) + tr.x + tr.width;
                    }

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
}

//Integral Image Light
//modify through muIntegralImage
//examinator->muIntegralImage Itlmg
//test
//opt #if win
//opt muCalcIntegralImage
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
    muCalcIntegralImage(inputData, Itlmg->sum, Itlmg->sqsum, Itlmg->imgSize);
    return Itlmg;
}

void muIntegral_LightRelease(muIntegralImg_t* Itlmg)
{
    free(Itlmg->sum);
    free(Itlmg->sqsum);
}

//SetImage Light -- Wait for learning program done

//Object Detection Light
void muObjectDetection_Light(muIntegralImg_t *Itlmg, muRect_t ScanROI, muSeq_t* Objects, MuSimpleDetector* cascade, double scaleFactor, muSize_t minSize, muSize_t maxSize)
{
    //Create result sequence
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
        
        muSize_t winSize = { muRound( cascade->orig_window_size.width * factor ),
                                muRound( cascade->orig_window_size.height * factor )};
        
        muRect_t rRect = { 0, 0, 0, 0 };

        startX = ScanROI.x;
        startY = ScanROI.y;
        endX = ScanROI.x+ScanROI.width - winSize.width;
        endY = ScanROI.y+ScanROI.height - winSize.height;

        if( winSize.width < minSize.width || winSize.height < minSize.height )
            continue;

        if ( winSize.width > maxSize.width || winSize.height > maxSize.height )
            break;

        muSetImagesForHaarClassifierCascade( cascade, Itlmg->sumSize, Itlmg->sum, Itlmg->sqsum, factor );

        for( iy = startY; iy < endY; iy+=ystep )
        {
            ixstep = ystep;
            //int result;
            for( ix = startX; ix < endX; ix += ixstep )
            {
                result = ctRunHaarClassifierCascade( cascade, Itlmg->sumSize, ix, iy, 10 );
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

}

//Object Detection Light
void muObjectDetection_SuperLight(muIntegralImg_t *Itlmg, muRect_t ScanROI, muSeq_t* Objects, MuSimpleDetector* cascade, muSize_t winSize)
{
    //Create result sequence
    muSize_t sumSize;
    double factor, tmp_factor;
    int iy, ix;
    int result, ixstep;
    int startX, startY;
    int endX, endY;
    double ystep;
	muRect_t rRect = { 0, 0, 0, 0 };

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
    
    winSize.width = muRound( cascade->orig_window_size.width * factor );
    winSize.height = muRound( cascade->orig_window_size.height * factor );

    startX = ScanROI.x;
    startY = ScanROI.y;
    endX = ScanROI.x+ScanROI.width - winSize.width;
    endY = ScanROI.y+ScanROI.height - winSize.height;

    muSetImagesForHaarClassifierCascade( cascade, Itlmg->sumSize, Itlmg->sum, Itlmg->sqsum, factor );

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
    }// for scanning y s
}

muSeq_t *muObjectDetection(muImage_t *img, MuSimpleDetector* cascade, double scaleFactor, muSize_t minSize, muSize_t maxSize)
{
	MU_8U *inputData; //Image data
	muSeq_t *rectList; //Result rectangle list
	muSize_t sumSize; //Size of integral image
	muSize_t imgSize; //Size of image
	int *sum;
	double *sqsum;
	int n_factors = 0;
	double factor;
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

    //Create result sequence
	rectList = muCreateSeq(sizeof(muRect_t));

	muCalcIntegralImage(inputData, sum, sqsum, imgSize);


	for( n_factors = 0, factor = 1;
             factor*cascade->orig_window_size.width < imgSize.width - 10 &&
			 factor*cascade->orig_window_size.height < imgSize.height - 10;
             n_factors++, factor *= scaleFactor );
	
	factor = 1;
	for( ; n_factors-- > 0; factor *= scaleFactor)
    {	
		const double ScanStep = factor > 2? factor: 2; //Scan step increase when window size increase after totalscalefactor is bigger than 2
		
		muSize_t winSize = { muRound( cascade->orig_window_size.width * factor ),
                                muRound( cascade->orig_window_size.height * factor )};
		
		muRect_t rRect = { 0, 0, 0, 0 };
        int startX = 0, startY = 0;
		int endX = muRound((imgSize.width - winSize.width) / ScanStep);
		int endY = muRound((imgSize.height - winSize.height) / ScanStep);

        if( winSize.width < minSize.width || winSize.height < minSize.height )
            continue;

        if ( winSize.width > maxSize.width || winSize.height > maxSize.height )
            break;

		muSetImagesForHaarClassifierCascade( cascade, sumSize, sum, sqsum, factor );

        for( iy = startY; iy < endY; iy++ )
        {
            y = muRound(iy*ScanStep);
			ixstep = 1;
            for( ix = startX; ix < endX; ix += ixstep )
            {
                x = muRound(ix*ScanStep);
                result = ctRunHaarClassifierCascade( cascade, sumSize, x, y, 5 );
                if( result > 0 )
				{
				    rRect.x = x;
				    rRect.y = y;
				    rRect.width = winSize.width;
				    rRect.height = winSize.height;
					muPushSeq(rectList, (MU_VOID *)&rRect);
				}
				ixstep = result != 0 ? 1 : 2;
            }
        }
	}
	
	free(sum);
	free(sqsum);

    return rectList;
}

/**Merge Function**/
/*MergeObjDistTH: OverlapTH - 2 means 1/2, 3 means 1/3*/
/*HitNum: TH for number of merged blocks*/
void muMergeRectangles(muSeq_t *Rectangles, int MergeObjDistTH, int HitNum)
{
    int i, j, MergedNum;
    int CrossArea, AreaMinX, AreaMaxX, AreaMinY, AreaMaxY;
    int RecMaxX1, RecMaxX2, RecMinX1, RecMinX2;
    int RecMaxY1, RecMaxY2, RecMinY1, RecMinY2;
    long X, Y, Wid, Hei;
    muSeqBlock_t *current1, *current2, *tmpBlock=NULL;
    muRect_t *rectp1, *rectp2;

    if(Rectangles != NULL)
    {
        current1 = Rectangles->first;
        while(current1 != NULL)
        {
            rectp1 = (muRect_t *)current1->data;
            X = RecMinX1 = rectp1->x;
            Y = RecMinY1 = rectp1->y;
            Wid = rectp1->width;
            Hei = rectp1->height;
            RecMaxX1 = RecMinX1 + Wid;
            RecMaxY1 = RecMinY1 + Hei;
            MergedNum = 1;
            current2 = current1->next;
            while(current2 != NULL)
            {
                rectp2 = (muRect_t *)current2->data;
                RecMinX2 = rectp2->x;
                RecMinY2 = rectp2->y;
                RecMaxX2 = RecMinX2 + rectp2->width;
                RecMaxY2 = RecMinY2 + rectp2->height;
                tmpBlock=NULL;
                //Sum of x, y, w, h; if overlapped
                if(!(RecMaxX2<=RecMinX1 || RecMinX2>=RecMaxX1 ||
                 RecMaxY2<=RecMinY1 || RecMinY2>=RecMaxY1 ))
                {
                    AreaMinX = RecMinX2 > RecMinX1 ? RecMinX2 : RecMinX1;
                    AreaMaxX = RecMaxX2 < RecMaxX1 ? RecMaxX2 : RecMaxX1;
                    AreaMinY = RecMinY2 > RecMinY1 ? RecMinY2 : RecMinY1;
                    AreaMaxY = RecMaxY2 < RecMaxY1 ? RecMaxY2 : RecMaxY1;
                    CrossArea = (AreaMaxX - AreaMinX)*(AreaMaxY - AreaMinY);
                    if(CrossArea!=0)
                    if( rectp1->width*rectp1->height<MergeObjDistTH*CrossArea && rectp2->width*rectp2->height<MergeObjDistTH*CrossArea)
                    {
                        /*rectp1->x = (rectp1->x + rectp2->x)/2;
                        rectp1->y = (rectp1->y + rectp2->y)/2;
                        rectp1->width = (rectp1->width + rectp2->width)/2;
                        rectp1->height = (rectp1->height + rectp2->height)/2;*/
                        // Aggressive Moving Merge
                        RecMinX1 = rectp2->x;
                        RecMinY1 = rectp2->y;
                        RecMaxX1 = RecMinX1 + rectp2->width;
                        RecMaxY1 = RecMinY1 + rectp2->height;
                        
                        X+=rectp2->x;
                        Y+=rectp2->y;
                        Wid+=rectp2->width;
                        Hei+=rectp2->height;
                        MergedNum++;
                        tmpBlock = current2;
                    }
                }
                current2 = current2->next;
                if(tmpBlock!=NULL)
                    muRemoveAddressNode(&Rectangles, tmpBlock);
            }

            tmpBlock = current1;
            current1 = current1->next;
            //save mean of rect to current1 data
            if(MergedNum > HitNum)
            {
                rectp1->x = X/MergedNum;
                rectp1->y = Y/MergedNum;
                rectp1->width = Wid/MergedNum;
                rectp1->height = Hei/MergedNum;
            }
            else
                muRemoveAddressNode(&Rectangles, tmpBlock);
        }
    }
}
