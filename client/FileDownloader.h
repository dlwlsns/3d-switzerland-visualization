#pragma once

#define _CRT_SECURE_NO_DEPRECATE
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
//#include <thread>
//#include <mutex>
//#include <chrono>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <json/json.h>
#include <fstream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

using namespace curlpp::options;

class FileDownloader
{
private:
	//std::vector<std::thread> download_threads;
public:
	std::string MyHTTPRequest(std::string url);

	Json::Value JsonParser(std::string mystringstream);

	const int MAX_DOWNLOADS = 5;
	//std::mutex m;

	// Function to extract the file name from a URL
	static std::string get_file_name(const std::string& url);

	// Function to download a single file
	void download_file(const std::string& url, const std::string& output_dir);

	static void clearFolder(const std::string& dir_path);

	void filedownloader(const std::vector<std::string>& urls);
};

