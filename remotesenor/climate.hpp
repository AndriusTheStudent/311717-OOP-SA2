// Developer note stack size needs to be increased to allow the allocation of memory for this double = 8 bytes

// 8 * 24 * 60 * 60 * 2 arrays of doubles = 1382400 bytes base allocation is 1MB 

// In VS2017 go to project > properties > System > and set Stack Reserve Size and Stack Commit Size to 2000000

// This prevents a stack overflow



#define maximum_readings 24 * 60 * 60

#define invalid_reading  -1000.0

#include <vector>

#include <chrono>  // System time

#include <stdexcept>



#define fakedata 1	 // NOTE:  Set to 1 use fake sensor values instead of the online values 0


#if fakedata == 1



#include "fakesensor.hpp"



#else



#include "http.h"



#endif





using namespace std;

using namespace chrono;





class Climate {



private:



#if fakedata == 1	

	FakeSensor sensorDevice;

#else

	Http sensorDevice;

#endif



protected:

	//sampleCount method

	int samples, totalSamples, currentSecond;
	float value;

	//declaring temperature and humidity arrays
	vector <float> temperatureVector;
	vector <float> humidityVector;



	

public:
	
	// Constructors

	Climate();



	// Utility

	void	clearSamples();



	// Sensor related

	long	readSensor();



	long sampleCount(long secs);



	long sampleTotal();



	double getHumidity(long sec);



	double averageHumidity(long lookBack);



	double maximumHumidity(long lookBack);



	double minimumHumidity(long lookBack);



	double getTemperature(long sec);



	double averageTemperature(long lookBack);



	double maximumTemperature(long lookBack);

	

	double minimumTemperature(long lookBack);



	system_clock::time_point StartTime;

};



// Constructor

Climate::Climate(): temperatureVector(1), humidityVector(1) {

	
	//get app start time
	temperatureVector.reserve(40);
	StartTime = std::chrono::system_clock::now();
	
}



void	Climate::clearSamples() {

	//temperatureVector.clear();
	//humidityVector.clear();
	//cout << "Samples cleared" << endl;
	//temperatureVector.resize(5);
	//humidityVector.resize(5);
	

}



long	Climate::readSensor() {
	
	

	//get sensor read method launch time

	try {
		sensorDevice.read_data();
		cout << " Vector size: " << temperatureVector.size() << endl;
		cout << " Capacity: " << temperatureVector.capacity() << endl;

		system_clock::time_point EndTime = std::chrono::system_clock::now();
		std::chrono::duration<double> Duration = EndTime - StartTime;;
		currentSecond = (int)Duration.count();

		temperatureVector.push_back(sensorDevice.get_temperature_in_c());
		humidityVector.push_back(sensorDevice.get_humidity());


		//cout << "Debugging information: " << "Temperature is " << temperatureVector[currentSecond] << "\nHumidity is : " << humidityVector[currentSecond] << "%" << endl;

		//store sensor readings in vector
		if (currentSecond < 1) {

			throw underflow_error("Less than a 1 second from last sample");

		}
		if (currentSecond > maximum_readings) {

			throw out_of_range("Over 24 hout limit!");

		}

		
		
		if (sensorDevice.get_temperature_in_c() == invalid_reading || sensorDevice.get_humidity() == invalid_reading) {

			throw runtime_error("Read attempt failure!");

		}
		
		
	}
	catch (const runtime_error &e) {
		cerr << "Error 1 occured: " << e.what() << endl;
	}
	catch (const out_of_range &e)
	{
		cerr << "Error 2 occured: " << e.what() << endl;
	}
	catch (const underflow_error &e) {
		cerr << "Error 3 occured: " << e.what() << endl;
	}
	catch (const invalid_argument &e) {
		cerr << "Error 4 occured: " << e.what() << endl;
	}
	catch (...) {
		cerr << "Unknown 5 Error" << endl;
	}
	/*for (vector<float>::iterator it = temperatureVector.begin(); it != temperatureVector.end(); it++)
	{
		cout << " "<<*it<<" " << flush;
	}*/
	return currentSecond;


	
}



	

long Climate::sampleCount(long secs) {

	try {
		if (secs > maximum_readings || secs < 1) {

			throw out_of_range("Out of range");

		}

		for (int n = secs; n >= 1; n--) {

			if (temperatureVector.at(n) != NULL) {

				samples++;
			}

			if (humidityVector.at(n) != NULL) {

				samples++;
			}

		}
		

	}
	catch (const out_of_range &e)
	{
		cerr << "Error occured: " << e.what() << endl;
	}

	return samples;
}







long Climate::sampleTotal() {

	for (auto it = temperatureVector.begin(); it != humidityVector.end(); ++it) {

		totalSamples++;

	}

	return totalSamples;

}



double Climate::getHumidity(long sec) {

	try {

		if (sec > maximum_readings || sec < 1) {

			throw out_of_range("Out of range");

		}

		value = humidityVector.at(sec);

		if (value == invalid_reading) {

			throw invalid_argument("No samples at specified time!");

		}

		

	}

	catch (const out_of_range &e)
	{
		cerr << "Error occured: " << e.what() << endl;
	}
	catch (const invalid_argument &e) {
		cerr << "Error occured: " << e.what() << endl;
	}
	
	return value;
}



double Climate::averageHumidity(long lookBack) {

	double avg;

	double sum = 0;

	if (lookBack > maximum_readings || lookBack < 1) {

		throw out_of_range("Out of range");

	}

	for (int i = lookBack; i >= 1; i--) {

		if (humidityVector[i] == NULL) {

			throw invalid_argument("No samples at specified time!");

		}

		sum += humidityVector[i];

	}

	avg = sum / lookBack;

	return avg;



}



double Climate::maximumHumidity(long lookBack) {

	if (lookBack > maximum_readings || lookBack < 1) {

		throw out_of_range("Out of range");

	}

	double max = humidityVector[lookBack];

	for (int i = lookBack; i >= 1; i--)

	{

		if (humidityVector[i] == NULL) {

			throw invalid_argument("No samples at specified time!");

		}

		if (humidityVector[i] > max) {

			max = humidityVector[i];

		}

	}

	return max;

}



double Climate::minimumHumidity(long lookBack) {

	if (lookBack > maximum_readings || lookBack < 1) {

		throw out_of_range("Out of range");

	}

	double min = humidityVector[lookBack];

	for (int i = lookBack; i >= 1; i--)

	{

		if (humidityVector[i] == NULL) {

			throw invalid_argument("No samples at specified time!");

		}

		if (humidityVector[i] < min) {

			min = humidityVector[i];

		}

	}

	return min;

}



double Climate::getTemperature(long sec) {

	if (sec > maximum_readings || sec < 1) {

		throw out_of_range("Out of range");

	}

	float value = temperatureVector.at(sec);

	if (value == NULL) {

		throw invalid_argument("No samples at specified time!");

	}

	return value;

}



double Climate::averageTemperature(long lookBack) {

	if (lookBack > maximum_readings || lookBack < 1) {

		throw out_of_range("Out of range");

	}

	double avg;

	double sum = 0;

	for (int i = lookBack; i >= 1; i--) {

		if (temperatureVector[i] == NULL) {

			throw invalid_argument("No samples at specified time!");

		}

		sum += temperatureVector[i];

	}

	avg = sum / lookBack;

	return avg;



}



double Climate::maximumTemperature(long lookBack) {

	if (lookBack > maximum_readings || lookBack < 1) {

		throw out_of_range("Out of range");

	}

	double max = temperatureVector[lookBack];

	for (int i = lookBack; i >= 1; i--)

	{

		if (temperatureVector[i] == NULL) {

			throw invalid_argument("No samples at specified time!");

		}

		if (temperatureVector[i] > max) {

			max = temperatureVector[i];

		}

	}

	return max;

}



double Climate::minimumTemperature(long lookBack) {

	if (lookBack > maximum_readings || lookBack < 1) {

		throw out_of_range("Out of range");

	}

	double min = temperatureVector[lookBack];

	for (int i = lookBack; i >= 1; i--)

	{

		if (temperatureVector[i] == NULL) {

			throw invalid_argument("No samples at specified time!");

		}

		if (temperatureVector[i] < min) {

			min = temperatureVector[i];

		}

	}

	return min;

} 
