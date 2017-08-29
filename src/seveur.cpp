#include "seveur.h"
#include <iostream>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <strings.h>
#include <stdlib.h>
//#include "pam_face_authentication.h"
#include "pam_face_defines.h"
//#include "webcamImagePaint.h"
//#include "opencvWebcam.h"
//#include "detector.h"
#include "verifier.h"

int sigflag;

int resquiescat(){wait();sigflag = 1;}


seveur::seveur()
{
    int listenfd = 0,socki = 0, sock=0;

      struct sockaddr_in serv_addr;

  //    sigset(SIGCHLD, resquiescat);


      char sendBuff[1025];
//      int numrv;

      listenfd = socket(AF_INET, SOCK_STREAM, 0);// creation de la socket serveur
   //   printf("socket retrieve success\n");

   //   memset(&serv_addr, '0', sizeof(serv_addr));

    //  memset(sendBuff, '0', sizeof(sendBuff));

      serv_addr.sin_family = AF_INET;
      serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      serv_addr.sin_port = htons(5000);

      bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));// on attache la socket a l'address du serveur

      if(listen(listenfd, 10) == -1){// starting listen
          printf("Failed to listen\n");
      }

      while(1)
        {
        /*  socki = accept(listenfd, (struct sockaddr*)NULL ,NULL); // accept awaiting request

       //   this->communicator();
          
               pthread_t sniffer_thread;
                 int *new_sock;
                 new_sock= (int*) malloc(1);
                 *new_sock = socki;


                 if( pthread_create( &sniffer_thread , NULL ,  seveur::communicator , (void*) new_sock) < 0)
                        {
                            perror("could not create thread");
                            continue;
                           // return 1;
                        }
                 puts("Handler assigned");


                 if (socki<0)
                 {
                     perror("accept failed");
                 }

                 
          close(socki);
          sleep(1);*/


/*
          this->sock = accept(listenfd, (struct sockaddr*)NULL ,NULL); // accept awaiting request

          this->communicator();
          close(this->sock);
          sleep(1);*/
          sigflag = 0;
          int childpid;
          this->sock = accept(listenfd, (struct sockaddr*)NULL ,NULL); // accept awaiting request
          if(this->sock<0)
          {
              //if(sigflag == 1)continue;
                                 /* if we got SIGCHLD during accept,
                                  * start a new iteration of the for loop */
              perror("accept error in telnetd");
              exit(-1);
          }

          if((childpid = fork()) < 0){

                       perror("fork error in Face_serveurd");
                       exit(-1);
                    }else if (childpid == 0){
         //     close(listenfd);
            /*  int i=0;
              while(i<25)
              {*/
                  this->communicator();
              //      this->communicator();
               /*   i++;
              }*/

          }

          close(this->sock);
      }

}


void seveur::communicator()
{
  //  this->sock = *(int*)socket_desc;

  //  this->stringsender("Message from server client");

    this->lire();

    char username[strlen(this->lu)];

    strcpy(username,this->lu);

  //  this->lire();

   // char msg[5]="";

    /*while(strcmp("quit",msg)!=0)
    {*/
        IplImage* img= this->IplImageRecv();

        uid_t id=this->get_id(username);

        int val=this->analyse(id,img);

//        std::cout<<val<<std::endl;

        this->send_int(val);

      //  this->lire();

      //  strcpy(msg,this->lu);
  //  }

//    std::cout<<username<<std::strlen(username)<<std::endl;

/*
    cvNamedWindow("Server", CV_WINDOW_AUTOSIZE);
    cvShowImage("Searver",img);
    cvWaitKey(3500);

    cvDestroyWindow("Server");*/



   // std::cout<<"ici"<<std::endl;
//return;
    exit(0);
}



int seveur::stringsender(char fi[])
{
    int a=strlen(fi);

    this->send_int(a);

    int n=write(this->sock,fi,std::strlen(fi));

    if(n<0)
        std::cout<<"error while sending"<<std::endl;
    return n;
}

int seveur::analyse(uid_t uid, IplImage* im)
{
    verifier* verif= new verifier(uid);

    int val=verif->verifyFace(im);

   /* if(val==0)
    {
        val=10;
    }*/

   // std::cout<<val<<std::endl;

    return val;
}

IplImage* seveur::IplImageRecv()
{
    int H=0,W=0;
    this->receive_int(&H);
 //   this->lire();
    this->receive_int(&W);


//    std::cout<<H<<" et "<<H<<std::endl;


    if(W==0)
        W=IMAGE_WIDTH;

    if(H==0)
      H=IMAGE_HEIGHT;

  //  cv::Mat  img = cv::Mat::zeros(480,640, CV_8UC3);
    cv::Mat  img = cv::Mat::zeros(H,W, CV_8UC3);

       int  imgSize = img.total()*img.elemSize();
       uchar sockData[imgSize];

      //Receive data here
       int bytes=0;

       for (int i = 0; i < imgSize; i = i + bytes) {
       if ((bytes = recv(this->sock, sockData +i, imgSize  - i, 0)) == -1) {
         //quit("recv failed", 1);

           std::cout<<"recv failed"<<std::endl;
           exit(0);
        }
       }

     // Assign pixel value to img


     int ptr=0;
     for (int i = 0;  i < img.rows; i++) {
      for (int j = 0; j < img.cols; j++) {
       img.at<cv::Vec3b>(i,j) = cv::Vec3b(sockData[ptr+ 0],sockData[ptr+1],sockData[ptr+2]);
       ptr=ptr+3;
       }
      }




     IplImage* im= cvCreateImage(cvSize(img.cols, img.rows), 8, 3);

     IplImage tmp=img;

     cvCopy(&tmp,im);


  /*   cvNamedWindow("Server", CV_WINDOW_AUTOSIZE);

             cvNamedWindow("Server", CV_WINDOW_AUTOSIZE);
             cvShowImage("Server",im);
 //            cv::imshow("Server",img);
             cvWaitKey(3500);

             cvDestroyWindow("Server");*/



     return im;
}

uid_t seveur::get_id(char* user)
{
    struct passwd *userStruct;

    userStruct = getpwnam(user);

    if(userStruct==NULL)
    {
        std::cout<<"Invalid user"<<std::endl;
        exit(0);
    }

    uid_t userID=userStruct->pw_uid;

    return userID;
}


void seveur::lire()
{
    int n = 0;

    this->receive_int(&n);

//    n--;

//    std::cout<<n<<std::endl;

    this->lu=(char*)std::realloc(this->lu,(n*sizeof(char)));

    if(this->lu==NULL)
    {
        std::cout<<"Allocation error"<<std::endl;
        return;
    }


  //  int a=0;
 //   recvBuff[0]='\0';
    this->lu[0]='\0';
/*
    if(n<10)
    {
        while((n= read(this->sock, this->lu, sizeof(this->lu))) > 0) {
               //     recvBuff[n]='\0';    // explicit null termination: updated based on comments
               //     std::strcat(this->lu,recvBuff) ;
                 //   recvBuff[0]='\0';        // clear the buffer : I am 99% sure this is not needed now
            //a=a+n;
            }
    }else
    {*/
    int a=this->readLine(this->lu,n);

    this->lu[n]='\0';
 //   }


      if( a < 0)
      {
        printf("\n Read Error \n");
      }

   //  return recvBuff;

}

int seveur::send_int(int num)
{/*
    int32_t conv = htonl(num);
    char *data = (char*)&conv;
    int left = sizeof(conv);
    int rc;
    do {
        rc = write(this->sock, data, left);
        if (rc < 0) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // use select() or epoll() to wait for the socket to be writable again
            }
            else if (errno != EINTR) {
                return -1;
            }
        }
        else {
            data += rc;
            left -= rc;
        }
    }
    while (left > 0);*/

    char buf[10] = "";

    sprintf( buf , "%d" , num );

    send( this->sock , buf , sizeof buf , 0 );

    return 0;
}

int seveur::receive_int(int *num)
{/*
    int32_t ret;
    char *data = (char*)&ret;
    data[0]='\0';
    int left = sizeof(ret);
    int rc;
    do {
        //rc=this->readLine(data, left);
        rc = read(this->sock, data, left);
   //     std::cout<<"sent"<<std::endl;

        if (ret <= 0) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // use select() or epoll() to wait for the socket to be readable again
            }
            else if (errno != EINTR) {
                return -1;
            }
        }
        else {
            data += rc;
            left -= rc;
        }
    }
    while (left > 0);
    *num = ntohl(ret);
    data[0]='\0';*/

    char buf[10] = "";

    recv( sock , buf , sizeof buf , 0 );

    sscanf( buf , "%d" , num );


    return 0;
}

int seveur::readLine(char data[],int maxlen)
{
   size_t len = 0;
   while (len < maxlen)
   {
      char c;
      int ret = read(this->sock, &c, 1);
      if (ret < 0)
      {
          data[len] = 0;
          return len; // EOF reached
      }
      if (c == '\n')
      {
          data[len] = 0;
          return len; // EOF reached
      }
      data[len++] = c;
   }
}



