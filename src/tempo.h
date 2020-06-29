#ifndef _TEMPO_H_
#define _TEMPO_H_
#include <sys/time.h>
#include<sys/resource.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iomanip>
#include <fstream>
#include <iterator>
#include <functional>
#include <math.h>
class tempo {
public:
    double relogio;
    double usuario;
    double sistema;

    tempo()  {
        getTime();
    }
    tempo(double r,double u,double s) {
        relogio = r;
        usuario = u;
        sistema = s;
    }

    friend tempo operator- (const tempo &t1, const tempo &t2) {
        return tempo(t1.relogio-t2.relogio,t1.usuario-t2.usuario, t1.sistema-t2.sistema);
    }

    friend ostream& operator<< (ostream& co,  const tempo &t) {
        co<< setiosflags(ios::left)<< setiosflags(ios::fixed)
        ;
        co<< t.relogio << "; "
        << t.usuario << "; "
        << t.sistema;

        return co;
    }
    void  getTime() {
        static timeval tim;
        static struct rusage  srusage;
        gettimeofday(&tim, NULL);
        getrusage(RUSAGE_SELF, &srusage);
        relogio = tim.tv_sec+(tim.tv_usec/1000000.0);
        sistema = srusage.ru_stime.tv_sec + srusage.ru_stime.tv_usec*0.000001;
        usuario = srusage.ru_utime.tv_sec + srusage.ru_utime.tv_usec*0.000001;
    }
};

#endif
