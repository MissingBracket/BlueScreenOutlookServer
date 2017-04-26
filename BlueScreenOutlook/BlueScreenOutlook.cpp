// BlueScreenOutlook.cpp : Defines the entry point for the console application.
//SERVER
#include "stdafx.h"
#pragma comment(lib, "ws2_32.lib")
#include<WinSock2.h>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

void iterateserver(SOCKET wire);
void pootis( );
vector<string> Mails;


bool alreadyin = false;

void SendMail(SOCKET wire, char* file) {
	FILE* f = fopen(file, "r"); char messg[256]; 
	send(wire, "+OK", sizeof("+OK"), NULL);
	while (!feof(f)) {
		fgets(messg, 256, f);
		//fscanf(f, "%s", &messg);
		send(wire, messg, sizeof(messg), NULL);
	}
	send(wire, "+0100", sizeof("+0100"), NULL);


	fclose(f);
}

bool SendData(SOCKET sock, void *buffer, int bufferLength)
{
	char *pbuffer = (char *)buffer;

	while (bufferLength > 0)
	{
		int n = send(sock, pbuffer, bufferLength, 0);
		if (n == SOCKET_ERROR)
		{
			return false;
		}

		pbuffer += n;
		bufferLength -= n;
	}

	return true;
}

bool SendLong(SOCKET sock, long value)
{
	value = htonl(value);
	return SendData(sock, &value, sizeof(value));
}

bool SendFile(SOCKET sock, FILE *f)
{
	fseek(f, 0, SEEK_END);
	long filesize = ftell(f);
	rewind(f);
	if (filesize == EOF)
		return false;
	if (!SendLong(sock, filesize))
		return false;
	if (filesize > 0)
	{
		char buffer[1024];
		do
		{
			size_t n = min(filesize, sizeof(buffer));
			n = fread(buffer, 1, n, f);
			if (n < 1)
				return false;
			if (!SendData(sock, buffer, n))
				return false;
			filesize -= n;
		} while (filesize > 0);
	}
	return true;
}

void RemoveMail(int wshere) {
	if (!Mails.empty()); {
		int k = 0;
		for (auto it = Mails.begin(); it != Mails.end(); it++) {
			if (k == wshere) {
				Mails.erase(it); return;
			}
			else k++;
		}
	}
}
void AccessMail(SOCKET wire) {
	char msg[128] = "Maildrop has 3 messages\n"; char status[8] = "Sending";
	send(wire, msg, strlen(msg), NULL); Sleep(50);
	string notifier;
	strcpy(msg, "MAIL#\tSubject\t\tAttachments\n");
	//Sleep(50);
	send(wire, msg, strlen(msg), NULL); Sleep(50);
	send(wire, "+OK", strlen("+OK"), NULL); //exit(1);
	*msg = { 0 };
	for (int i = 0; i < Mails.size(); i++) {
		notifier = to_string(i); notifier += "\t"; notifier += Mails[i]; notifier += "\0";
		cout << "\nNotifier:\t" << notifier<<endl;
		strcpy(msg, notifier.c_str());
		cout << "\nMsg:\t" << msg << endl;
		//	cout<<i+1<<"\tSubject:\t" << Mails[i]<<endl;
		send(wire, msg, strlen(msg), NULL); Sleep(50);
		
		//	send(wire, t, sizeof(t), NULL);
		notifier.clear(); *msg = { 0 };
	}Sleep(50);
	send(wire, "-ERR", strlen("-ERR"), 0);
}

void sAccessMail(SOCKET wire) {
	char MessBuff[128]; char MB2[128];
	//cout <<(char)Mails.size();
	string notifier = "You got "; notifier += to_string(Mails.size()); notifier += " Messages InBox\0";
	cout << notifier << endl;
	strcpy(MessBuff, notifier.c_str());
	//send(wire,"109",sizeof("109"),NULL);
	cout << endl << strlen(MessBuff) << endl;
	///send(wire, MessBuff, strlen(MessBuff), NULL); cout << "Sent at 99: "<<MessBuff<<endl;
	send(wire, "You got shit emails, burh", strlen("You got shit emails, burh"), NULL);
	strcpy(MB2, "Mail#\tSubject\tSender\t\tAttachments\n");
	send(wire, MB2, strlen(MB2), NULL); cout << "sent at 100\n";
	char t[8] = "Sending";
	send(wire, t, strlen(t), NULL); 
	for (int i = 0; i < Mails.size(); i++) {
		notifier = to_string(i); notifier += "\t"; notifier += Mails[i]; notifier += "\0";
		strcpy(MessBuff, notifier.c_str());
	//	cout<<i+1<<"\tSubject:\t" << Mails[i]<<endl;
		send(wire, MessBuff, strlen(MessBuff), NULL);
	//	send(wire, t, sizeof(t), NULL);
		notifier.clear(); *MessBuff = { 0 };
	}
	strcpy(MessBuff, "-Sending"); send(wire, MessBuff, strlen(MessBuff), NULL); exit(1);
}

void AuthenticateClient(SOCKET wire) {
	bool HasAccess = false;
	char Pass[30] = { 0 }, User[30] = { 0 };
	char *MOTC = "BlueScreen Outlook v 13.37\0";
	char msg[5] = { 0 };
	//send(wire, MOTC, strlen(MOTC), NULL);
	recv(wire, User, sizeof(User), NULL); cout << strlen(User )<< endl;
	send(wire, "+OK, client selected", strlen("+OK, client selected"), NULL);
	recv(wire, Pass, sizeof(Pass), NULL); cout << Pass << endl;
	if ((!strcmp(User, "Wire")) && (!strcmp(Pass, "Shark"))) {
		char LoginMess[256] = "+OK "; 
		send(wire, LoginMess, strlen(LoginMess), NULL);
		HasAccess = true;
	}
	else {
		char LoginMess[256] = "-ERR";
	send(wire, LoginMess, strlen(LoginMess), NULL);
	
	}
	if (HasAccess) {
		alreadyin = true;
		recv(wire, msg, sizeof(msg), NULL);
		AccessMail(wire);
	}
}

void iterateserver(SOCKET wire) {
	char st[8] = { 0 };
	while (true) {
		recv(wire, st, sizeof(st), NULL);
		if (strstr(st, "APOP") != NULL) {
			printf("here");
			if(!alreadyin) AuthenticateClient(wire); 
			else { send(wire, "-ERR", sizeof("-ERR"), NULL); return; }
			*st = { 0 };
		}

		else if (strstr(st, "RETR") != NULL) {
			if (st[5] - '0' >= Mails.size()) { send(wire, "-ERR", strlen("-ERR"), NULL); iterateserver(wire); }
			printf("Command is: %s with id of: %d\n", st, st[5] - '0');
			printf("Selected mail: %s\n", Mails[st[5] - '0'].c_str());
			FILE *file = fopen(Mails[st[5] - '0'].c_str(), "rb");
			if (file != NULL)
			{

				send(wire, "+OK", strlen("+OK"), NULL); cout << "sent\n";
				send(wire, Mails[st[5] - '0'].c_str(), strlen(Mails[st[5] - '0'].c_str()), NULL);
				SendFile(wire, file);

				fclose(file);
				FILE *fil = fopen("yee.mp4", "rb");
				if (fil != NULL)
				{
					SendFile(wire, fil);
					fclose(fil);
				}
				//SendMail(wire, nm);
			}
			else { send(wire, "-ERR", strlen("-ERR"), NULL); }


			
			*st = { 0 };
		}
		else if (strstr(st, "DELE") != NULL) {
			printf("Command is: %s with id of: %d\n", st, st[5] - '0');
			RemoveMail(st[5] - '0'); AccessMail(wire);
			*st = { 0 };
		}
		else if (strstr(st, "QUIT") != NULL) {
			printf("Command is: %s ", st); alreadyin = false;
			send(wire, "+OK bye bye", strlen("+OK bye bye"), NULL);
			return;
		}
		else {
			send(wire, "-ERR", strlen("-ERR"), NULL);
		}
		if (wire == NULL)return;
	}
}

void citerateserver(SOCKET wire) {
	bool inloop = true; char exc[128] = { 0 }; bool instate = 0;
	while (inloop&&wire!=0) {
		recv(wire, exc, sizeof(exc), NULL);

		if (strcmp(exc, "APOP") == 0) {
			instate = true; while (instate){
				AuthenticateClient(wire); instate = false;
			}
		}
		else if (strcmp(exc, "RETR") == 0) {
			int k; char nm[64] = { 0 };
			recv(wire, nm, sizeof(nm), NULL);// k=nm[0];
			//printf("Number?: %d\n", k);
			
			FILE *file = fopen(nm, "rb");
				if (file != NULL)
				{
					
					send(wire, "+OK\0", strlen("+OK\0"), NULL); cout << "sent\n";
					
					SendFile(wire, file);
					
					fclose(file);
					FILE *fil = fopen("yee.mp4", "rb");
					if (fil != NULL)
					{
						SendFile(wire, fil);
						fclose(fil);
					}
					//SendMail(wire, nm);
				}
				else { send(wire, "-ERR", strlen("-ERR"), NULL);}
			
		}
		else if (strcmp(exc, "DELE") == 0) {
			cout << "Got DELE\t";
			char nm[8] = { 0 }; int k;
			recv(wire, nm, sizeof(nm), NULL);
			printf("Got message: %s\n", nm);
			printf("%d\n", nm[5] - '0');
			//cout << Mails[nm[5] - '0'];
			exit(1);
			
			
			printf("\nOurString: %c\n", nm[0]);
			k = nm[0]; printf("we here");
			printf("\n%d\n", k);
			//cout << Mails[k];
			RemoveMail(k);
			AccessMail(wire);
		}
		else if (strcmp(exc, "rd") == 0) {
			char bf[16] = { 0 }; recv(wire, bf, sizeof(bf), NULL);
			SendMail(wire, bf);
			FILE *fil = fopen("yee.mp4", "rb");
			if (fil != NULL)
			{
				SendFile(wire, fil);
				fclose(fil);
			}
		}
		else if (strcmp(exc, "dank") == 0) {
			char beef[16] = { 0 };
			recv(wire, beef, sizeof(beef), NULL);
			printf("%s\n", beef);
			if (strstr(beef, "memes") != NULL) {
				printf("Got Fine Reading\n");
			}
		}
		else if (strcmp(exc, "QUIT") == 0) {
			inloop = false; return;
		}
		else if (strcmp(exc, "ShutDown") == 0)exit(1);
	}
}

int main(int argc, char* argv[])
{

	Mails.push_back("email.txt");
	Mails.push_back("DankMemes.txt");
	Mails.push_back("NigerianPrince.txt");
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0) {
		MessageBoxA(NULL, "WINSOCK At line 165", "ERROR", MB_OK | MB_ICONERROR);
		exit(1);
	}
	SOCKADDR_IN addr;
	int addrlen = sizeof(addr);
	//Dla LocalHost'a 
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //127.0.0.1 inaddrany
	//Dla sieci	
	//addr.sin_addr.s_addr = INADDR_ANY; //127.0.0.1 inaddrany
	addr.sin_port = htons(110);//1111
	addr.sin_family = AF_INET;

	SOCKET slisten = socket(AF_INET, SOCK_STREAM, NULL);
	bind(slisten, (SOCKADDR*)&addr, sizeof(addr));
	listen(slisten, SOMAXCONN);
	
		SOCKET NewConn;
		while (true) {
			cout << "Server Waiting for client\n";
		NewConn = accept(slisten, (SOCKADDR*)&addr, &addrlen);
		if (NewConn == 0) {
			printf("Connection Failed");
		}
		else {
			printf("GotClient\n");
			send(NewConn, "+OK", strlen("+OK"), NULL);
			/*char MOTC[256] = "Need Authentication ";
			send(NewConn, MOTC, sizeof(MOTC), NULL);*/
			//pootis(NewConn);
			iterateserver(NewConn);
			//1rem	AuthenticateClient(NewConn);

		}
	}
    return 0;
}
