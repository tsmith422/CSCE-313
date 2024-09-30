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

	Added edits as an example to base starter code
	Kyle Lang
	Date: 9/18/2024
*/
#include "common.h"
#include "stdlib.h"
#include "FIFORequestChannel.h"
#include <sys/wait.h>
#include <chrono>

using namespace std;

// Task 2.1: Single Data Point transfer
// Base code already provides an implementation for this task
void requestDataPoint(FIFORequestChannel &chan, int p, double t, int e)
{
	char buf[MAX_MESSAGE];
	datamsg x(p, t, e); // Request patient data point

	memcpy(buf, &x, sizeof(datamsg));
	chan.cwrite(&x, sizeof(datamsg)); // Directly write datamsg into pipe
	double reply;
	chan.cread(&reply, sizeof(double));

	cout << "For person " << p << ", at time " << t << ", the value of ecg " << e << " is " << reply << endl;
}

// Task 2.2: 1000 Data Point transfer into received/x1.csv
// Request 1000 data points from server and write results into received/x1.csv with same file format as the patient {1-15}.csv files
void requestData(FIFORequestChannel &chan, int p)
{
	// Open x1.csv file under ./received/
	ofstream ofs("./received/x1.csv");

	// Iterate 1000 times
	double t = 0.0;
	for (int i = 0; i < 1000; ++i)
	{
		// Write time into x1.csv (Time is 0.004 second deviations)
		datamsg msg1(p, t, 1); // Request ecg1

		// Write ecg1 datamsg into pipe
		chan.cwrite(&msg1, sizeof(datamsg));

		// Read response for ecg1 from pipe
		double ecg1;
		chan.cread(&ecg1, sizeof(double));

		// Write ecg1 value into x1.csv
		ofs << t << "," << ecg1 << ",";

		datamsg msg2(p, t, 2); // Request ecg2

		// Write ecg2 datamsg into pipe
		chan.cwrite(&msg2, sizeof(datamsg));

		// Read response for ecg2 from pipe
		double ecg2;
		chan.cread(&ecg2, sizeof(double));

		// Write ecg2 value into x1.csv
		ofs << ecg2 << endl;

		t += 0.004; // Increment time
	}

	// CLOSE YOUR FILE
	ofs.close();
}

// Task 3: Request File from server
// Request a file under BIMDC/ given the file name, by sequentially requesting data chunks from the file and copying into a new file of the same name into received/
void requestFile(FIFORequestChannel &chan, const string &fname)
{
	using namespace std::chrono;
	auto start = high_resolution_clock::now();

	filemsg fm(0, 0); // Request file length message
	int len = sizeof(filemsg) + fname.size() + 1;
	char *buf2 = new char[len];

	memcpy(buf2, &fm, sizeof(filemsg));
	strcpy(buf2 + sizeof(filemsg), fname.c_str());
	chan.cwrite(buf2, len);

	__int64_t file_length;
	chan.cread(&file_length, sizeof(__int64_t));
	cout << "The length of " << fname << " is " << file_length << endl;

	ofstream ofs("./received/" + fname, ios::binary);

	__int64_t offset = 0;
	while (offset < file_length)
	{
		__int64_t chunk_size = min(file_length - offset, (__int64_t)MAX_MESSAGE);
		fm = filemsg(offset, chunk_size);
		memcpy(buf2, &fm, sizeof(filemsg));
		chan.cwrite(buf2, len);

		char *data = new char[chunk_size];
		chan.cread(data, chunk_size);
		ofs.write(data, chunk_size);
		delete[] data;

		offset += chunk_size;
	}

	ofs.close();
	delete[] buf2;

	auto end = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(end - start).count();
	cout << "Time taken to transfer " << fname << ": " << duration << " ms" << endl;
}

// Task 4: Request a new FIFORequestChannel
// Send a request to the server to establish a new FIFORequestChannel, and use the servers response to create the client's FIFORequestChannel
// Client must now communicate over this new RequestChannel for any data point or file transfers
void openChannel(FIFORequestChannel &chan, FIFORequestChannel *&chan2)
{
	MESSAGE_TYPE m = NEWCHANNEL_MSG;
	// Write new channel message into pipe
	chan.cwrite(&m, sizeof(MESSAGE_TYPE));

	// Read response from pipe (Can create any static sized char array that fits server response, e.g. MAX_MESSAGE)
	char newPipeName[MAX_MESSAGE];
	chan.cread(newPipeName, MAX_MESSAGE);

	// Create a new FIFORequestChannel object using the name sent by server
	chan2 = new FIFORequestChannel(newPipeName, FIFORequestChannel::CLIENT_SIDE);
}

int main(int argc, char *argv[])
{
	int opt;
	int p = 1;
	double t = 0.0;
	int e = 1;
	string filename = "";
	int m_buff = MAX_MESSAGE;
	bool c = false;
	vector<FIFORequestChannel *> channels;

	// Add other arguments here   |   Need to add -c, -m flags. BE CAREFUL OF getopt() NOTATION
	while ((opt = getopt(argc, argv, "p:t:e:f:m:c")) != -1)
	{
		switch (opt)
		{
		case 'p': // patient id
			p = atoi(optarg);
			break;
		case 't': // time in seconds
			t = atof(optarg);
			break;
		case 'e': // ecg number
			e = atoi(optarg);
			break;
		case 'f': // filename
			filename = optarg;
			break;
		case 'm': // specifies the size of the buffer for communication
			m_buff = atoi(optarg);
			break;
		case 'c': // flag, if present, creates a new communication channel
			c = true;
			break;
		}
	}

	pid_t server_process = fork();

	if (server_process < 0)
	{
		return 1;
	}

	if (server_process == 0)
	{
		if (m_buff != MAX_MESSAGE)
		{
			char *args[] = {(char *)"./server", (char *)"-m", (char *)to_string(m_buff).c_str(), nullptr};
			execvp(args[0], args);
			return 1;
		}
		else
		{
			char *args[] = {(char *)"./server", nullptr};
			execvp(args[0], args);
			return 1;
		}
	}

	FIFORequestChannel chan("control", FIFORequestChannel::CLIENT_SIDE);
	FIFORequestChannel *chan2 = nullptr;

	// Task 4:
	// Request a new channel (e.g. -c)
	// Should use this new channel to communicate with server (still use control channel for sending QUIT_MSG)
	if (c)
		openChannel(chan, chan2);

	// Task 2.1 + 2.2:
	// Request data points
	if (p)
	{
		if (t)
			requestDataPoint(chan, p, t, e); // (e.g. './client -p 1 -t 0.000 -e 1')

		else
			requestData(chan, p); // (e.g. './client -p 1')
	}
	// Task 3:
	// Request files (e.g. './client -f 1.csv')
	if (!filename.empty())
	{
		requestFile(chan, filename);
	}

	// Task 5:
	//  Closing all the channels
	MESSAGE_TYPE m = QUIT_MSG;
	chan.cwrite(&m, sizeof(MESSAGE_TYPE));

	if (chan2)
	{
		chan2->cwrite(&m, sizeof(MESSAGE_TYPE));
		delete chan2;
	}

	// Wait on children
	wait(nullptr);
	return 0;
}
