#include "download.h"

int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    if (dltotal <= 0) {
        return 0;
    }

    int progress = static_cast<int>(100.0 * dlnow / dltotal);
    std::cout << "\rDownloading: " << progress << "%";
    std::cout.flush();

    if (progress == 100) {
        std::cout << std::endl;
    }

    return 0;
}

time_t get_local_timestamp(const std::string &file_path) {
    struct stat buf;
    if (stat(file_path.c_str(), &buf) == 0) {
        return buf.st_mtime;
    }
    return 0;
}


time_t get_remote_timestamp() {
    std::string url = "https://thredds.met.no/thredds/dodsC/metpplatest/met_forecast_1_0km_nordic_latest.nc.dds";
    std::string command = "curl -s -I " + url + " | grep -i 'Last-Modified' | awk '{ print substr($0, index($0,$2)) }'";
    char buffer[128];
    std::string result = "";
    std::cout << command << std::endl;

    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("Failed to open pipe");
    }

    while (fgets(buffer, 128, pipe) != nullptr) {
        result += buffer;
    }

    auto pclose_result = pclose(pipe);
    if (pclose_result == -1) {
        throw std::runtime_error("Failed to close pipe");
    }

    if (result.empty()) {
        return -1;
    }

    std::tm tm = {};
    std::istringstream ss(result);
    ss >> std::get_time(&tm, "%a, %d %b %Y %H:%M:%S %Z");

    if (ss.fail()) {
        return -1;
    }

    return std::mktime(&tm);
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

bool download_file(const std::string &output_path) {
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    std::string url = "https://thredds.met.no/thredds/fileServer/metpplatest/met_forecast_1_0km_nordic_latest.nc";
    std::cout << "Downloading file" << std::endl;
    if (curl) {
        FILE *fp;
        fp = fopen(output_path.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);

        if (res != CURLE_OK) {
            return false;
        }
        return true;
    }
    return false;
}

bool download_if_newer(const std::string &output_path) {
    time_t remote_time = get_remote_timestamp();
    time_t local_time = get_local_timestamp(output_path);
    std::cout << "local time: " << local_time <<  " remote time: " << remote_time << std::endl;

    if (remote_time > local_time) {
        return download_file(output_path);
    }

    return false;
}

