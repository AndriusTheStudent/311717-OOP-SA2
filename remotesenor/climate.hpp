// Developer note stack size needs to be increased to allow the allocation of memory for this double = 8 bytes

// 8 * 24 * 60 * 60 * 2 arrays of doubles = 1382400 bytes base allocation is 1MB 

// In VS2017 go to project > properties > System > and set Stack Reserve Size and Stack Commit Size to 2000000

// This prevents a stack overflow



#define maximum_readings 24 * 60 * 60

#define invalid_reading  -1000.0

#include <vector>;

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

	int samples, totalSamples;

	//declaring temperature and humidity arrays
	size_t size = 60;


	

public:
	
	//vector <float> temperatureVector;
	//vector <float> humidityVector;



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


vector <float> temperatureVector(60);
vector <float> humidityVector(60);
// Constructor

Climate::Climate() {

	
	//get app start time

	StartTime = std::chrono::system_clock::now();
	


}



void	Climate::clearSamples() {

	temperatureVector.clear();
	humidityVector.clear();
	cout << "Samples cleared" << endl;
	cout <<"Vector size: "<< temperatureVector.size()<< endl;
	cout << "Vector capacity: " << temperatureVector.capacity();
	if (temperatureVector.empty()) {
		cout << " Vector empty" << endl;
	}
	else {
		cout << " Vector not empty" << endl;
	}


}



long	Climate::readSensor() {
	//vector <float> temperatureVector(60);
	//vector <float> humidityVector(60);

	//get sensor read method launch time

	system_clock::time_point EndTime = std::chrono::system_clock::now();

	sensorDevice.read_data();

	//calculate time passed since launch of app

	std::chrono::duration<double> Duration = EndTime - StartTime;;

	int currentSecond = (int)Duration.count();



	//store sensor readings in vector
	temperatureVector.push_back(sensorDevice.get_temperature_in_c());
	humidityVector.push_back(sensorDevice.get_humidity());
	
	/*
	if (currentSecond < 1) {

		throw underflow_error("Less than a 1 second from last sample");

	}

	if (currentSecond > maximum_readings) {

		throw out_of_range("Over 24 hout limit!");

	}

	if (sensorDevice.get_temperature_in_c() == NULL || sensorDevice.get_humidity() == NULL) {

		throw runtime_error("Read attemt failure!");

	}
	*/

	// This line is purely for your debugging and can be removes/commented out in the final code.

		cout << endl << "Debugging information : " << "Temperature is " << sensorDevice.get_temperature_in_c() << " in degrees C " << sensorDevice.get_humidity() << "% humidity" << endl;

		cout << " Element at index: " << currentSecond << " Temp: "<<temperatureVector[currentSecond]<<" Humidity: "<<humidityVector[currentSecond]<< endl;	
		cout << "Size of vector: " << temperatureVector.size() << endl;

		for (vector<float>::iterator it = temperatureVector.begin(); it != temperatureVector.end(); it++) {
			cout << *it << endl;
		}


		

	return currentSecond;

}



	

long Climate::sampleCount(long secs) {



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
	return samples;

}







long Climate::sampleTotal() {

	for (auto it = temperatureVector.begin(); it != humidityVector.end(); ++it) {

		totalSamples++;

	}

	return totalSamples;

}



double Climate::getHumidity(long sec) {

	if (sec > maximum_readings || sec < 1) {

		throw out_of_range("Out of range");

	}

	

	float value = humidityVector.at(sec);

	if (value == NULL) {

		throw invalid_argument("No samples at specified time!");

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
