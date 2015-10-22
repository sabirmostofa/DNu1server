////////////////////////// Noise Generator Model ////////////////////////////
// pseudo-random generator
// http://en.wikipedia.org/wiki/Park%E2%80%93Miller_random_number_generator
uint32_t lcg_seed = 1;
uint32_t lcg_rand(){
	//lcg_seed = ((uint64_t)lcg_seed * 279470273UL) % 4294967291UL;
	lcg_seed = rand();
    return lcg_seed;
}
// pseudo-random in [0,1]
double randf(){
	return lcg_rand() / (double)0xFFFFFFFFUL;
}
const double PI = 3.14159265;
// pseudo-Rauschen gaussian
double randf_gauss(){
	double mu = 0.0; // mittelwert
	double sigma = 0.25; //!! varianz 0.45; try a values from 0.25 to 0.55
	return mu + sigma*sqrt(-2.0*log(randf())) * cos(2*PI*randf());
}
/****************** Analog Kanal Modell mit Rauschen *******************
 *                am Eingang ein originelles Bit aus den Datastream
 *                das Bit ist in analoge Grösse umgewandelt, gedämmt
 *                und mit dem Rauschen zusammengemischt.
 *                Das Ergebnis wird wieder als ein Bit dargestellt
 *                Am Ausgang ein Bit mit möglicher Störung             
 ***********************************************************************/
char analog_kanal_modell(char inputbit){   //add noise to the bit stream
	double input_signal_level = 0.1;
    char outputbit;
    double in, noise, out;
    /////////////////////  Digital to Analog conversion //////////////////
    if (inputbit != '0'){
        in = +input_signal_level;
    }else{
        in = -input_signal_level;
    }
    ///////////////////  Störungen im Kanal //////////////////    
    noise = 0.1 * randf_gauss(); // pseudo-zufällige Zahlen, Gauss Verteilung
	out = in + noise;			 // Analogsignal mit Rauschen
    /////////////////// Analog to Digital conversion //////////////////
    if(out>0.0)	
        outputbit = '1';
    else
        outputbit = '0';
    return outputbit;
}
