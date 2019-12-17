#include "randoms.h"

randoms::randoms()
{
    load_randoms();
    nextrandom=0;
}

quint16 randoms::gen_random()
{
    if (nextrandom==65535)
                {
                    nextrandom=0;
                   load_randoms();
                }
    return random_array[nextrandom++];

}

randoms::~randoms()
{


}

//Load random numbers
void randoms::load_randoms()
{
    //Seed pseudorandom numbers
    qsrand(static_cast<uint>(QTime::currentTime().msec()));

    //Load randoms into program - get random
    int seed_random = qrand();

    //Make sure 16 bit random so not too large for the randoms file
    double seedoffset = (static_cast<double>(seed_random)/static_cast<double>(RAND_MAX))*(65536.);
    int intseedoffset = static_cast<int>(seedoffset);

    QFile rfile("://resources/randoms.dat");
    if (!rfile.exists())QMessageBox::warning(nullptr,"Damn","Error loading randoms. Contact RJG in the hope he can sort this out.");
    rfile.open(QIODevice::ReadOnly);
    rfile.seek(intseedoffset);

    // multiply by two here as char * is 8bit, and randoms are 16 - no error if array is unfilled, randoms are just zero...
    int i=rfile.read((char *)random_array,(65536*2));
    if (i!=(65536*2)) QMessageBox::warning(nullptr,"Oops","Failed to read 131072 bytes from randoms file - random numbers may be compromised - Contact RJG");
    rfile.close();
}
