/*
	Author of the starter code
	Yifan Ren
	Department of Computer Science & Engineering
	Texas A&M University
	Date: 9/15/2024

	Please include your Name, UIN, and the date below
	Name: Taylor Smith
	UIN: 732003356
	Date: 9/17/24
*/
#include "common.h"
#include "FIFORequestChannel.h"

using namespace std;

int main(int argc, char *argv[])
{
	int opt;
	int p = 1;
	double t = 0.0;
	int e = 1;
	string filename = "";

	// Add other arguments here
	int m = MAX_MESSAGE;
	while ((opt = getopt(argc, argv, "p:t:e:f:")) != -1)
	{
		switch (opt)
		{
		case 'p':
			p = atoi(optarg);
			break;
		case 't':
			t = atof(optarg);
			break;
		case 'e':
			e = atoi(optarg);
			break;
		case 'f':
			filename = optarg;
			break;
		case 'm':
			m = atoi(optarg);
			break;
		}
	}

	// Task 1:
	// Run the server process as a child of the client process
	// give args for the server
	// server needs './server', '-m', '<val for -m arg>', 'NULL'
	// fork
	// In the child, run execvp using the server args.

	FIFORequestChannel chan("control", FIFORequestChannel::CLIENT_SIDE);

	// Task 4:
	// Request a new channel

	// Task 2:
	// Request data points
	// Single datapoint, only run p, t, e != -1
	char buf[MAX_MESSAGE]; // 256
	datamsg x(1, 0.0, 1);  // change from hardcoding to user's values

	memcpy(buf, &x, sizeof(datamsg));
	chan.cwrite(buf, sizeof(datamsg)); // question
	double reply;
	chan.cread(&reply, sizeof(double)); // answer
	cout << "For person " << p << ", at time " << t << ", the value of ecg " << e << " is " << reply << endl;

	// Else, if p != -1, request 1000 datapoints
	// Loop over 1st 1000 lines
	// send request for ecg 1
	// send request for ecg 2
	// write line to received/x1.csv

	// Task 3:
	// Request files
	// sending a nonsense message, need to change
	filemsg fm(0, 0);
	string fname = "1.csv";

	int len = sizeof(filemsg) + (fname.size() + 1);
	char *buf2 = new char[len];
	memcpy(buf2, &fm, sizeof(filemsg));
	strcpy(buf2 + sizeof(filemsg), fname.c_str());
	chan.cwrite(buf2, len); // I want the file length

	delete[] buf2;
	__int64_t file_length;
	chan.cread(&file_length, sizeof(__int64_t));
	cout << "The length of " << fname << " is " << file_length << endl;

	// Task 5:
	//  Closing all the channels
	MESSAGE_TYPE m = QUIT_MSG;
	chan.cwrite(&m, sizeof(MESSAGE_TYPE));
}
