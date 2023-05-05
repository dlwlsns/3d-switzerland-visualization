#include "fileDownloader.h"

std::string FileDownloader::MyHTTPRequest(std::string url) {
	std::stringstream os;
	try
	{
		curlpp::Easy foo;
		foo.setOpt(new curlpp::options::Url(url));
		foo.setOpt(new curlpp::options::SslVerifyPeer(false));
		foo.setOpt(new curlpp::options::WriteStream(&os));

		// send our request to the web server
		foo.perform();
	}

	catch (curlpp::RuntimeError& e)
	{
		std::cout << e.what() << std::endl;
	}

	catch (curlpp::LogicError& e)
	{
		std::cout << e.what() << std::endl;
	}
	return os.str();
}

Json::Value FileDownloader::JsonParser(std::string mystringstream) {
	const std::string rawJson = mystringstream;
	const auto rawJsonLength = static_cast<int>(rawJson.length());
	constexpr bool shouldUseOldWay = false;
	JSONCPP_STRING err;
	Json::Value root;

	if (shouldUseOldWay) {
		Json::Reader reader;
		reader.parse(rawJson, root);
	}
	else {
		Json::CharReaderBuilder builder;
		const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
		if (!reader->parse(rawJson.c_str(), rawJson.c_str() + rawJsonLength, &root,
			&err)) {
			std::cout << "error" << std::endl;
		}
	}
	return root;
}

// Function to extract the file name from a URL
std::string FileDownloader::get_file_name(const std::string& url) {
	size_t pos = url.rfind('/');
	if (pos != std::string::npos) {
		return url.substr(pos + 1);
	}
	return url;
}

curlpp::Easy foo;
// Function to download a single file
void FileDownloader::download_file(const std::string& url, const std::string& output_dir) {
	std::string output_file = output_dir + "/" + get_file_name(url);
	std::ofstream myfile;

	if (fs::exists(output_file)) {
		std::cout << "Already present: " << output_file << std::endl;
		return;
	}
	
	std::cout << "Downloading " << get_file_name(url);

	FILE* fp = fopen(output_file.c_str(), "wb");

	if (fp) {
		foo.setOpt(new curlpp::options::Url(url));
		foo.setOpt(new curlpp::options::SslVerifyPeer(false));
		foo.setOpt(new curlpp::options::WriteFile(fp));

		// send our request to the web server
		foo.perform();

		fclose(fp);
	}

	std::cout << " Done" << std::endl;
}

void FileDownloader::clearFolder(const std::string& dir_path) {
	for (const auto& entry : fs::directory_iterator(dir_path))
		fs::remove_all(entry.path());
}

void FileDownloader::filedownloader(const std::vector<std::string>& urls) {
	// Directory to save the downloaded files
	std::string output_dir = "./files";
	
	// Start downloading the files
	int num_downloads = 0;
	
	for (int i = 0; i < urls.size(); i++) {
		download_file(urls[i], output_dir);
	}
}

