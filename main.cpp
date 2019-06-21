#include "Kernel.h"
#include "Utility.h"

int main()
{
	Kernel* k = Kernel::getInstance();
	k->initialize();
	while (true)
	{
		cout << "yly@os:" << k->curdir << "$ ";
		char input[100];
		cin.getline(input, 100);
		vector<char*> result = Utility::parseCmd(input);
		if (result.size() > 0)
		{
			if (strcmp(result[0], "cd") == 0)
			{
				if (result.size() > 1)
				{
					k->cd(result[1]);
					if (k->error == Kernel::NOTDIR)
						cout << result[1] << ": Not a directory" << endl;
					else if (k->error == Kernel::NOENT)
						cout << result[1] << ": No such file or directory" << endl;
				}
				else
					k->cd("/");
			}
			else if (strcmp(result[0], "fformat") == 0)
			{
				k->format();
				k->initialize();
			}
			else if (strcmp(result[0], "mkdir") == 0)
			{
				if (result.size() > 1)
				{
					k->mkdir(result[1]);
					if (k->error == Kernel::NOENT)
						cout << "No such file or directory" << endl;
					if (k->error == Kernel::ISDIR)
						cout << result[1] << ": Is a directory" << endl;
				}
				else
					cout << "mkdir: missing operand" << endl;
				
			}
			else if (strcmp(result[0], "ls") == 0)
			{
				k->ls();
			}
			else if (strcmp(result[0], "fopen") == 0)
			{
				int fd;
				if (result.size() > 1)
				{
					fd = k->open(result[1], 511);
					if (k->error == Kernel::NO_ERROR)
						cout << "fd = " << fd << endl;
					else if (k->error == Kernel::ISDIR)
						cout << result[1] << ": Is a directory" << endl;
					else if (k->error == Kernel::NOENT)
						cout << result[1] << ": No such file or directory" << endl;
				}	
				else
					cout << "fopen: missing operand" << endl;
				
			}
			else if (strcmp(result[0], "fcreate") == 0)
			{
				int fd;
				if (result.size() > 1)
				{
					fd = k->create(result[1], 511);
					
					if (k->error == Kernel::NOENT)
						cout << "No such file or directory" << endl;
					if (k->error == Kernel::ISDIR)
						cout << result[1] << ": Is a directory" << endl;
					if (k->error == Kernel::NO_ERROR)
						cout << "fd = " << fd << endl;
				}
				else
					cout << "fcreate: missing operand" << endl;
				
			}
			else if (strcmp(result[0], "fclose") == 0)
			{
				if (result.size() > 1)
					k->close(atoi(result[1]));
				else
					cout << "fclose: missing operand" << endl;
			}
			else if (strcmp(result[0], "fread") == 0)
			{
				int actual;
				if (result.size() > 2) {
					char* buf;
					buf = new char[atoi(result[2])];
					buf[0] = '\0';
					actual = k->fread(atoi(result[1]), buf, atoi(result[2]));
					if (actual == -1)
					{
						if (k->error == Kernel::BADF)
							cout << atoi(result[1]) << ": Wrong fd" << endl;
					}
					else
					{
						if (actual > 0)
						{
							for (int i = 0; i < actual; i++)
								//cout << buf[i];  
								//cout << buf[i] << endl;
								printf("%c", buf[i]);
							cout << endl;
						}
						cout << "Actually read " << actual << " bytes." << endl;
					}
					delete buf;
				}
				else
					cout << "fread: missing operand" << endl;
			}
			else if (strcmp(result[0], "fwrite") == 0)
			{
				int actual;
				if (result.size() > 3) 
				{
					if (atoi(result[2]) > strlen(result[3]))
					{
						cout << "nbytes can\'t be larger than the length of the string" << endl;
						continue;
					}
					actual = k->fwrite(atoi(result[1]), result[3], atoi(result[2]));
					if (actual == -1)
					{
						if (k->error == Kernel::BADF)
							cout << atoi(result[1]) << ": Wrong fd" << endl;
					}
					else
					{
						cout << "Actually write " << actual << " bytes." << endl;
					}
				}
				else
				{
						cout << "fwrite: missing operand" << endl;
				}	
			}
			else if (strcmp(result[0], "fseek") == 0)
			{
				if (result.size() > 3)
				{
					if (atoi(result[3]) >= 0 && atoi(result[3]) <= 5)
					{
						k->fseek(atoi(result[1]), atoi(result[2]), atoi(result[3]));
						if (k->error == Kernel::BADF)
							cout << atoi(result[1]) << ": Wrong fd." << endl;
					}
					else
						cout << result[3] << ": Wrong ptrname." << endl;
					
				}
				else
				{
					cout << "fseek: missing operand" << endl;
				}
			}
			else if (strcmp(result[0], "fdelete") == 0)
			{
				if (result.size() > 1)
				{
					k->fdelete(result[1]);
					if (k->error == Kernel::NOENT)
						cout << result[1] << ": No such file or directory" << endl;
				}
				else
				{
					cout << "fdelete: missing operand" << endl;
				}
			}
			else if (strcmp(result[0], "fmount") == 0)
			{
				if (result.size() > 2)
				{
					k->fmount(result[1], result[2]);
					if (k->error == Kernel::NOENT)
						cout << result[2] << ": No such file or directory" << endl;
					else if (k->error == Kernel::NOOUTENT)
						cout << result[1] << ": No such file or directory" << endl;
					else if (k->error == Kernel::ISDIR)
						cout << result[2] << ": Is a directory" << endl;
				}
				else
				{
					cout << "fmount: missing operand" << endl;
				}
			}
			else if (strcmp(result[0], "quit") == 0) 
			{
				cout << "Bye!" << endl;
				k->clear();
				break;
			}
			else {
				cout << "command \'" << result[0] << "\' not found" << endl;
			}
		}
		cout << endl;
	}
	return 0;
}