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
#include <sys/wait.h>

using namespace std;

int main(int argc, char *argv[])
{
	int opt;
	int p = 1;
	double t = 0.0;
	int e = 1;
	string filename = "";
	int m = MAX_MESSAGE;
	bool new_chan = false;
	vector<FIFORequestChannel *> channels;

	// Add other arguments here
	while ((opt = getopt(argc, argv, "p:t:e:f:m:c")) != -1)
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
		case 'c':
			new_chan = true;
			break;
		}
	}

	// Task 1:
	// Run the server process as a child of the client process
	// give args for the server
	// server needs './server', '-m', '<val for -m arg>', 'NULL'
	// fork
	// In the child, run execvp using the server args.
	char *args[] = {(char *)"./server", (char *)"-m", (char *)m, nullptr};
	pid_t server_process = fork();
	if (server_process == 0)
	{
		execvp(args[0], args);
	}

	FIFORequestChannel cont_chan("control", FIFORequestChannel::CLIENT_SIDE);
	channels.push_back(&cont_chan);

	if (new_chan)
	{
		// send newchannel request to the server
		MESSAGE_TYPE nc = NEWCHANNEL_MSG;
		cont_chan.cwrite(&nc, sizeof(MESSAGE_TYPE));
		// create a variable to hold the name
		// create the response from the server
		// call the FIFORequestChannel constructor with the name from the server
		// Push the new channel into the vector
	}

	FIFORequestChannel chan = *(channels.back());

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

	__int64_t file_length;
	chan.cread(&file_length, sizeof(__int64_t));
	cout << "The length of " << fname << " is " << file_length << endl;

	// char* buf3 = create buffer of size buff capacity (m)

	// loop over the segments in the file filesize / buff capacity(m)
	// create filemsg instance
	filemsg *file_req = (filemsg *)buf2;
	// file_req->offset = set offset in the file
	// file req->length = set the length. Be careful of the last segment
	// send the request (buf2)
	chan.cwrite(buf2, len);
	// receive the response
	// cread into buf3 length file_req->len
	// write buf3 into file: received/filename

	delete[] buf2;
	// delete[] buf3;

	// if necessary, close and delete the new channel
	if (new_chan)
	{
		// do your close and deletes
	}

	// Task 5:
	//  Closing all the channels
	MESSAGE_TYPE m = QUIT_MSG;
	chan.cwrite(&m, sizeof(MESSAGE_TYPE));
	waitpid(server_process, nullptr, 0);
}
