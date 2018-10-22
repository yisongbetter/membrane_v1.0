#include "detector.h"

int classify (Mat img, int x_min, int y_min, int x_max, int y_max)
{
    
    int flag;

    Mat img2;
    img.copyTo(img2);
 //   Mat crop = img2(Rect(x_min,y_min,x_max-x_min,y_max-y_min));

    int w=img.cols;
    int h=img.rows;
    int rw=w-x_min-1>5?5:w-x_min-1;
    int rh=h-y_min-1>5?5:h-y_min-1;
    Mat crop = img2(Rect(x_min,y_min,rw,rh));
    //imshow("coped image",crop);
    Scalar big = mean( img2 );
    Scalar small = mean( crop );
    float value1=big.val[0]+big.val[1]+big.val[2],value2=small.val[0]+small.val[1]+small.val[2];
    if (value1<=value2) {
        flag=0;
    }
    else{
        flag=1;
    }
   
    return flag;
}

float mean_of_crop (Mat img, int x_min, int y_min, int x_max, int y_max,int thresh)
{
    int start_x = x_min-30<0 ? x_min:x_min-30;
    int start_y = y_min-30<0 ? y_min:y_min-30;
    int end_x = x_max+30>thresh ? x_max:x_max+30;
    int end_y = y_max+30>thresh ? y_max:y_max+30;
    Mat img2;
    img.copyTo(img2);
    Mat crop = img2(Rect(start_x,start_y,end_x-start_x,end_y-start_y));
   // imshow("croped image",crop);
   // Scalar big = mean( img2 );
    Scalar small = mean( crop );
    return small.val[0]+small.val[1]+small.val[2];
   // float value1=big.val[0]+big.val[1]+big.val[2],value2=small.val[0]+small.val[1]+small.val[2];

}

int find_min (vector<int> vec)
{
    int min =10000;
    for(int i=0;i<vec.size();++i){
        if (vec[i]<min) min=vec[i];
    }
    return min==10000?0:(min<0?0:(min>999?999:min));
}
int find_max (vector<int> vec)
{
    int max = -1;
    for(int i=0;i<vec.size();++i){
        if (vec[i]>max) max=vec[i];
    }
    return max==-1?1000:(max<0?0:(max>999?999:max));
}

#define find_max_point(a,max)  ((a)>(max)?(max)=(a):(max)=(max))
#define find_min_point(a,min)  ((a)<(min)?(min)=(a):(min)=(min))
int defects_detector(Mat input,my_detector* p_detector)
{
  
       // Mat src = imread(in, 0);
      //  Mat dst = imread(in,1);
        Mat src;
        cvtColor(input,src,COLOR_BGR2GRAY);
       // Mat src1 = src,src2 = src,src3 = src;
        Mat dst1, dst2, dst3;
        Mat image1,image2,image3;
       // my_detector detector;
        //resize(src1,image1,Size(1000,1000),0,0);
        resize(src,image1,Size(1000,1000),0,0);
        resize(src,image2,Size(3000,3000),0,0);
        resize(src,image3,Size(200,200),0,0);

        int flag;
    
        // Create and LSD detector with standard or no refinement.
    #if 1
        Ptr<LineSegmentDetector> ls1 = createLineSegmentDetector(LSD_REFINE_ADV);
        Ptr<LineSegmentDetector> ls2 = createLineSegmentDetector(LSD_REFINE_ADV);
        Ptr<LineSegmentDetector> ls3 = createLineSegmentDetector(LSD_REFINE_ADV);
    #else
        Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_NONE);
    #endif
        double start = double(getTickCount());
        vector<Vec4f> lines_std1;
        vector<Vec4f> lines_std2;
        vector<Vec4f> lines_std3;

        vector<int> x_min,y_min,x_max,y_max;
        int x_min_f,y_min_f,x_max_f,y_max_f;
        // Detect the lines
        ls1->detect(image1, lines_std1);
        ls2->detect(image2, lines_std2);
        ls3->detect(image3, lines_std3);

        int max1_x=-1,max1_y=-1,min1_x=10000,min1_y=10000;
        int max2_x=-1,max2_y=-1,min2_x=10000,min2_y=10000;
        int max3_x=-1,max3_y=-1,min3_x=10000,min3_y=10000;


        int count1 = lines_std1.size();
       
            for (int i = 0; i < count1; i++) {
                Vec4f p = lines_std1[i];
                //cout<<"p[0]= "<<p[0]<<"p[1]= "<<p[1]<<"p[2]= "<<p[2]<<"p[3]= "<<p[3]<<endl;

#if 1
               find_max_point(p[0],max1_x);
                find_max_point(p[2],max1_x);
                find_min_point(p[0],min1_x);
                find_min_point(p[2],min1_x);

                find_max_point(p[1],max1_y);
                find_max_point(p[3],max1_y);
                find_min_point(p[1],min1_y);
                find_min_point(p[3],min1_y);



#else
                if (p[0] >= max1_x) {
                    max1_x = p[0];
                }
                if (p[2] >= max1_x) {
                    max1_x = p[2];
                }
                if (p[0] <= min1_x) {
                    min1_x = p[0];
                }
                if (p[2] <= min1_x) {
                    min1_x = p[2];
                }
                if (p[1] >= max1_y) {
                    max1_y = p[1];
                }
                if (p[3] >= max1_y) {
                    max1_y = p[3];
                }
                if (p[1] <= min1_y) {
                    min1_y = p[1];
                }
                if (p[3] <= min1_y) {
                    min1_y = p[3];
                }
#endif
            }
        
        int count2 = lines_std2.size();
        if (count2!=0) {
            for (int i = 0; i < count2; i++) {
                Vec4f p = lines_std2[i];
               // cout << p[0] << endl;
                #if 1
                 find_max_point(p[0],max2_x);
                find_max_point(p[2],max2_x);
                find_min_point(p[0],min2_x);
                find_min_point(p[2],min2_x);

                find_max_point(p[1],max2_y);
                find_max_point(p[3],max2_y);
                find_min_point(p[1],min2_y);
                find_min_point(p[3],min2_y);

                #else
                if (p[0] >= max2_x) {
                    max2_x = p[0];
                }
                if (p[2] >= max2_x) {
                    max2_x = p[2];
                }
                if (p[0] <= min2_x) {
                    min2_x = p[0];
                }
                if (p[2] <= min2_x) {
                    min2_x = p[2];
                }
                if (p[1] >= max2_y) {
                    max2_y = p[1];
                }
                if (p[3] >= max2_y) {
                    max2_y = p[3];
                }
                if (p[1] <= min2_y) {
                    min2_y = p[1];
                }
                if (p[3] <= min2_y) {
                    min2_y = p[3];
                }
                #endif
                //line(image2, Point(p[0],p[1]), Point(p[2],p[3]), Scalar(0, 255, 255), 3);
            }
        }

        int count3 = lines_std3.size();
        if (count3!=0) {
            for (int i = 0; i < count3; i++) {
                Vec4f p = lines_std3[i];
                //cout << p[0] << endl;
                #if 1
                 find_max_point(p[0],max3_x);
                find_max_point(p[2],max3_x);
                find_min_point(p[0],min3_x);
                find_min_point(p[2],min3_x);

                find_max_point(p[1],max3_y);
                find_max_point(p[3],max3_y);
                find_min_point(p[1],min3_y);
                find_min_point(p[3],min3_y);

                #else
                if (p[0] >= max3_x) {
                    max3_x = p[0];
                }
                if (p[2] >= max3_x) {
                    max3_x = p[2];
                }
                if (p[0] <= min3_x) {
                    min3_x = p[0];
                }
                if (p[2] <= min3_x) {
                    min3_x = p[2];
                }
                if (p[1] >= max3_y) {
                    max3_y = p[1];
                }
                if (p[3] >= max3_y) {
                    max3_y = p[3];
                }
                if (p[1] <= min3_y) {
                    min3_y = p[1];
                }
                if (p[3] <= min3_y) {
                    min3_y = p[3];
                }
                #endif
            }
        }
        double duration_ms = (double(getTickCount()) - start) * 1000 / getTickFrequency();
        std::cout << "It took " << duration_ms << " ms." << std::endl;
        Mat test;
        image1.copyTo(test);
        // Show found lines
        // Mat drawnLines(image);
        // ls->drawSegments(drawnLines, lines_std);
        if (max1_x!=-1){
            max1_x=max1_x>>1;max1_y=max1_y>>1;min1_x=min1_x>>1;min1_y=min1_y>>1;
            
            x_min.push_back(min1_x);
            y_min.push_back(min1_y);
            x_max.push_back(max1_x);
            y_max.push_back(max1_y);
           // int mean1 = mean_of_crop (image1, min1_x, min1_y, max1_x, max1_y,1000);
            //rectangle(image1, Point(min1_x,min1_y), Point(max1_x,max1_y), Scalar(0, 255, 255), 1);;
        }

        if (max2_x!=-1){
            max2_x=max2_x/6.0;max2_y=max2_y/6.0;min2_x=min2_x/6.0;min2_y=min2_y/6.0;
            x_min.push_back(min2_x);
            y_min.push_back(min2_y);
            x_max.push_back(max2_x);
            y_max.push_back(max2_y);
           // int mean2 = mean_of_crop (image2, min2_x, min2_y, max2_x, max2_y,3000);
           // rectangle(image1, Point(min2_x,min2_y), Point(max2_x,max2_y), Scalar(0, 255, 255), 1);
        }

        if (max3_x!=-1){
            max3_x=max3_x*2.5;max3_y=max3_y*2.5;min3_x=min3_x*2.5;min3_y=min3_y*2.5;
            x_min.push_back(min3_x);
            y_min.push_back(min3_y);
            x_max.push_back(max3_x);
            y_max.push_back(max3_y);
          //  int mean3 = mean_of_crop (image3, min3_x, min3_y, max3_x, max3_y,200);
          //  rectangle(image1, Point(min3_x,min3_y), Point(max3_x,max3_y), Scalar(0, 255, 255), 1);
        }
        if (max1_x==-1 && max2_x==-1 && max3_x==-1){
          //  cout<<"This image has no defect~~~~~~~~~~"<<endl;
            //resize(image1,dst1,Size(500,500),0,0);
            //if_detects=0;
            //p_detector->my_flag=0;
            sprintf(p_detector->my_label,"No Defect");
            printf("no f\n");
            return 0;
            //imshow("No Defect", dst1);
        }
        else{


           // cout<<"This image has defects!!!!!!!!"<<endl;

           
           // if_detects = 1;
            p_detector->my_flag=1;
            x_min_f = find_min(x_min);
            y_min_f = find_min(y_min);
            x_max_f = find_max(x_max);
            y_max_f = find_max(y_max);
            flag = classify(image1,x_min_f,y_min_f,x_max_f,y_max_f);

         
           // cout<<"flag= "<<flag<<endl;
            int a=(x_min_f-15)<0?10:(x_min_f-15);
            int b=(y_min_f-15)<0?10:(y_min_f-15);
            int c=(x_max_f+15)>=500?490:(x_max_f+15);
            int d=(y_max_f+15)>=500?490:(y_max_f+15);

            printf("%d %d %d %d\n",a*2,b*2,c*2,d*2);

          
            resize(input,dst2,Size(500,500),0,0);
            rectangle(dst2, Point(a,b), Point(c,d), Scalar(0, 0,255), 2);
          
            //resize(image1,dst1,Size(500,500),0,0);
            //imshow("Result1", dst1);
            
          
            if (flag==0){
          
                sprintf(p_detector->my_label,"light leak!");
                p_detector->defects = dst2;
          
                //imshow("light leak", dst2);
               // imshow("defects detected!", dst2);

            }
            if (flag==1){
          
                sprintf(p_detector->my_label,"stain!");
                p_detector->defects = dst2;
          

                //imshow("stain", dst2);
               // imshow("defects detected!", dst2);

            }
            return 1;
            
        }

        /*
        cout<<max1_x<<"  "<<min1_x<<"  "<<max1_y<<"  "<<min1_y<<endl;
        cout<<max2_x<<"  "<<min2_x<<"  "<<max2_y<<"  "<<min2_y<<endl;
        cout<<max3_x<<"  "<<min3_x<<"  "<<max3_y<<"  "<<min3_y<<endl;
        */
      //  waitKey(0);
      //  destroyAllWindows();
           


 }

