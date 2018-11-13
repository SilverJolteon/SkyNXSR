#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <cstdlib>

using namespace std;

string version = "v1.2";

class savefile{
	private:
		string filepath;
		string sav0;
		string sav1;
		string datetime;
		string ID;
		string name;
		string name_str;
		int level;
		short int nameLength;
		string location;
		short int locationLength;
		string playtime;
		short int playtimeLength;
		
		string str2hex(string str){ //Converts alphanumeric string to hex string
			ostringstream ret;
			for(auto i = 0; i < str.length(); i++){
				ret << hex << setfill('0') << setw(2) << uppercase << (int)str[i];
			}
			return ret.str();
		}
		
		void getTime(){ //Gets current time in YYYYMMDDHHMMSS format
			time_t t = time(nullptr);
			char now[16];
			strftime(now, sizeof(now), "%Y%m%d%H%M%S", localtime(&t));
			datetime = now;
		}
		
		void getID(){ //Prompts user for ID
			string ID_str;
			do{
				system("@cls");
				cout << "Skyrim Switch Save Renamer " << version <<"\n        by SilverJolteon" << endl << endl;
				cout << "Create a 4 character-long ID: ";
				cin >> ID_str;
			}while(ID_str.length() != 4);
			ID = str2hex(ID_str);
		}
		
		void getName(){ //Reads Character Name
			fstream file(filepath, ios::in | ios::binary);
			file.seekg(0x19, file.beg);
			file.read(reinterpret_cast<char*>(&nameLength), 2);
			file.seekg(0x1B, file.beg);
			char* buffer = new char[nameLength];
			file.read(buffer, nameLength);
			file.close();
			for(auto i = 0; i < nameLength; i++) name_str += buffer[i];
			name = str2hex(name_str);
			delete[] buffer;
		}
		
		void getLocation(){ //Reads save location
			fstream file(filepath, ios::in | ios::binary);
			file.seekg(0x1F+nameLength, file.beg);
			file.read(reinterpret_cast<char*>(&locationLength), 2);
			file.seekg(0x21+nameLength, file.beg);
			char* buffer = new char[locationLength];
			file.read(buffer, locationLength);
			file.close();
			for(auto i = 0; i < locationLength; i++) location += buffer[i];
			location = str2hex(location);
			delete[] buffer;
		}
		
		void getPlaytime(){ //Reads playtime
			fstream file(filepath, ios::in | ios::binary);
			file.seekg(0x21+nameLength+locationLength, file.beg);
			file.read(reinterpret_cast<char*>(&playtimeLength), 2);
			file.seekg(0x23+nameLength+locationLength, file.beg);
			char* buffer = new char[playtimeLength];
			file.read(buffer, playtimeLength);
			file.seekg(0x1B+nameLength, file.beg);
			file.read(reinterpret_cast<char*>(&level), 4);
			file.close();
			for(auto i = 0; i < playtimeLength; i++) playtime += buffer[i];
			delete[] buffer;
		}
		
	public:
		savefile(string fp):filepath(fp){ //
			getTime();
			getID();
			getName();
			getLocation();
			getPlaytime();
		}
		
		void renameFile(){ //Renames save file and creates blank one
			sav0 += datetime + "_Save1_" + ID + "_0_" + name + "_"; 
			sav1 += datetime + "_";
			if(location.length() > 26-name.length()){ //Checks location string's length
				sav0 += location.substr(0, 26-name.length());
				sav1 += location.substr(26-name.length(), location.length());
			}
			else{
				sav0 += location;
			}
			sav0 += ".sav0";
			char level_str[4];
			itoa(level, level_str, 10);
			sav1 += "_" + playtime + "_";
			sav1 += level_str;
			sav1 += "_1.sav1";
			
			string newFileName = name_str + "_" + playtime;
			
			string command = "if not exist " + newFileName + " mkdir " + newFileName;
			system(command.c_str());
			rename(filepath.c_str(), sav0.c_str());
			command = "move " + sav0 + " " + newFileName + ">nul";
			system(command.c_str());
			fstream blank(newFileName + "/" + sav1, ios::out | ios::binary);
			blank.close();
		}
};

bool checkFile(string filepath){
	string header;
	fstream file(filepath, ios::in | ios::binary);
	file.seekg(0, file.beg);
	char* buffer = new char[0xD];
	file.read(buffer, 0xD);
	file.close();
	for(auto i = 0; i < 0xD; i++) header += buffer[i];
	delete[] buffer;
	if(header == "TESV_SAVEGAME"){
		return true;
	}
	return false;
}

int main(int argc, char* argv[]){
	if(argv[1] != NULL){ //Renames file as drag&drop argument
		if(!checkFile(argv[1])){
			cout << "Save not recognized!" << endl << endl;
			system("pause");
			return 0;
		}
		savefile save(argv[1]);
		save.renameFile();
	}
	else{ //Renames file by running program
		string path, filepath; //Needs two separate strings to work
		cout << "Skyrim Switch Save Renamer " << version << "\n        by SilverJolteon" << endl << endl;
		cout << "To rename your save, simply drag it onto this program." << endl;
		getline (cin, filepath);
		path = filepath;
		if(path[0] = 0x22 && path[path.length()-1] == 0x22){
			path = path.substr(1, path.length()-2);
			if(!checkFile(path)){
				cout << "Save not recognized!" << endl << endl;
				system("pause");
				return 0;
			}
			savefile save(path);
			save.renameFile();
		}
		else{
			if(!checkFile(filepath)){
				cout << "Save not recognized!" << endl << endl;
				system("pause");
				return 0;
			}
			savefile save(filepath);
			save.renameFile();
		}
	}
	cout << "Successfully renamed save!" << endl << endl;
	system("pause");
	return 0;
}