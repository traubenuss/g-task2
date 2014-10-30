/*
 * Utils.h
 *
 *  Created on: Jul 3, 2014
 *      Author: test
 */

#ifndef UTILS_H_
#define UTILS_H_


#include <string>
#include <vector>
#include <dirent.h>
#include <opencv2/opencv.hpp>

#include "tclap/CmdLine.h"

#if defined _WIN32
  #include <conio.h>
  #include <direct.h>
  #define MKDIR(path) _mkdir(path); // TODO: check if windows makes directory
  #define FOLDER_CHAR  "\\"
#elif defined __linux__
  #include <sys/types.h>
  #include <sys/stat.h>
  #define MKDIR(path) mkdir(path, 0777); // notice that 777 is different than 0777
  #define FOLDER_CHAR  "/"
#endif

#define PAUSE_AND_SHOW(image) \
    std::cout << "  Paused - press any key" << std::endl; \
    cv::namedWindow("DEBUG"); \
    cv::imshow("DEBUG", image); \
    cv::waitKey(); \
    cv::destroyWindow("DEBUG");



using namespace std;
using namespace cv;


inline vector<string> splitStringByDelimiter(const string& src0, const string& delimiter)
{
	vector<string> output;

	string src = src0; // make a non-const copy
	size_t pos = 0;
	string token;
	while ((pos = src.find(delimiter)) != string::npos)
	{
	    token = src.substr(0, pos);
	    output.push_back(token);
	    src.erase(0, pos + delimiter.length());
	}
	output.push_back(src);

	return output;
}

template <typename T>
inline void linspace(std::vector<T>& result, T start, T end, int N)
{
    result.clear();

    if(N <= 1)
    {
        result.push_back(start);
        return;
    }

    for(int i = 0; i <= N-2; i++)
    {
        T cur_val = start + i*(end-start)/((T)N - 1.0);
        result.push_back(cur_val);
    }
    result.push_back(end);
}

template <typename T>
inline T randRange(T low = 0, T high = 1)
{
    return (T)((((double)(high-low))*((double)rand()/RAND_MAX)))+low;
}



struct CommandParams
{
    enum ExecMode {STATIC, VIDEO, LIVE};
	ExecMode exec_mode_;
    string str_datapath_;
};

inline void parseCmd(int argc, char* argv[], CommandParams& params)
{
	try {
		// Command Line
        TCLAP::CmdLine cmd("Usage: -m [static,live] [-d imgdir]. on static mode, define where the input images are located", ' ', "1.0");

		// Command Arguments
        TCLAP::ValueArg<std::string> mArg("m","mode","Execution mode, either 'static', 'video' or 'live'.",true,"","string");
        TCLAP::ValueArg<std::string> dArg("d","data","Data directory where the image files are located (static mode) or path to video file (video mode)",false,"","string");
		cmd.add( mArg );
		cmd.add( dArg );

		// Parse the argv array.
		cmd.parse( argc, argv );

		// Get the values parsed by each arg.

        string mode_str        = mArg.getValue();
        params.str_datapath_  = dArg.getValue();

		std::cout << "Command line:" << endl <<
				     "-------------" << endl;
		cout <<
                    " mode:             " << mode_str << endl <<
                    " img directory:    " << params.str_datapath_ << endl << endl;

        if(mode_str != "static" && mode_str != "video" && mode_str != "live")
        {
            cout << "mode specifier not recognized! exiting..." << endl;
            exit(-1);
        }

        if(mode_str == "static")
        {
            // append '/' (or '\') if missing
            if(params.str_datapath_.compare(params.str_datapath_.size()-1,1,FOLDER_CHAR) != 0)
                params.str_datapath_ += FOLDER_CHAR;

            params.exec_mode_ = CommandParams::STATIC;
        }
        if(mode_str == "video")
            params.exec_mode_ = CommandParams::VIDEO;
        if(mode_str == "live")
            params.exec_mode_ = CommandParams::LIVE;
	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        cout << "parsing failed. call --help to view parameter specifications" << endl;
        exit(-1);
	}
}


inline bool getFileList(string directory, vector<string>& filelist)
{
	//--------------------------------------------
	// READ IMAGE DIRECTORY FILELIST and filter *.jpg *.png

    if(directory.compare(directory.size()-1,1,FOLDER_CHAR) != 0)
        directory += FOLDER_CHAR;

    filelist.clear();
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(directory.data())) != NULL)
    {
        // print all the files and directories within directory
        while ((ent = readdir(dir)) != NULL)
        {
            std::string filename = directory + ent->d_name;
            if(filename.compare(filename.size()-4,4,".png") == 0 ||
               filename.compare(filename.size()-4,4,".jpg") == 0 ||
               filename.compare(filename.size()-4,4,".bmp") == 0)
            {
            	filelist.push_back(ent->d_name);
//                std::cout << "File: " << ent->d_name << std::endl << flush;
            }
        }
        closedir(dir);
    }
    else
    {
        /* could not open directory */
        std::cerr << "ERROR: Image directory doesn't exist!" << std::endl;
        return false;
    }

    sort(filelist.begin(), filelist.end());

    return true;
}




#endif /* UTILS_H_ */
