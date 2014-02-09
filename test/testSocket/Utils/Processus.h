#ifndef PROCESSUS_H
#define PROCESSUS_H
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <thread>
#include <cstdlib>

#include "../Serveur.h"
#include "Conf.h"

namespace utils
{
    void runServeur();
    void runCrawl();
    void runCommande();
    bool cmd();


    class Processus
    {
        public:
            Processus();
            virtual ~Processus();
            void stopAll();
        protected:
        private:

    };
}

#endif // PROCESSUS_H