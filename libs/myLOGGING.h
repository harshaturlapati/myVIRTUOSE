std::string GetTimestamp(time_t now) {
	tm* ltm = localtime(&now);
	std::string year = std::to_string(1900 + ltm->tm_year);
	std::string month = std::to_string(1 + ltm->tm_mon);
	std::string day = std::to_string(ltm->tm_mday);
	std::string hour = std::to_string(ltm->tm_hour);
	std::string min = std::to_string(ltm->tm_min);
	std::string sec = std::to_string(ltm->tm_sec);
	std::string timestamp = year + "_" + month + "_" + day + "_" + hour + min + sec + ".csv";
	return timestamp;
}


//
//void write_to_file_v3(int64_t unix_epoch[], double time_log[], int duration, int data_count, time_t now, std::string file_name)
//{
//	std::cout << "Writing to file..." << std::endl;
//
//	get_exp_folder_v2();
//	std::string base_path = exp_folder;
//
//	std::string timestamp = GetTimestamp(now);
//
//	std::ofstream log_file(base_path + file_name + timestamp + ".csv");
//
//	if (log_file.is_open())
//	{
//		log_file << "Time(ms),Time(s),Unix_epoch(ns),"
//			<< "UDP_f1,UDP_f2,UDP_f3,UDP_f4,UDP_f5,UDP_f6" << ","
//			<< "X,Y,Z,qx,qy,qz,qw" << ","
//			<< "f1,f2,f3,f4,f5,f6" << ","
//			<< "Index\n";
//
//		for (int i = 0; i < duration * 1000 && i < data_count - 1; ++i)
//		{
//			log_file << (time_log[i] - time_log[0]) / 1000 << ",";
//			log_file << (time_log[i] - time_log[0]) / 1000000 << ",";
//			log_file << unix_epoch[i] << ",";
//
//			// "UDP_f1,UDP_f2,UDP_f3,UDP_f4,UDP_f5,UDP_f6" << ","
//			for (int j = 0; j < dim_f; ++j)
//			{
//				log_file << UDP_f_log[i][j] << ",";
//			}
//
//			for (int j = 0; j < dim_CART; ++j)
//			{
//				log_file << Virtuose_POS_log[i][j] << ",";
//			}
//
//			for (int j = 0; j < dim_f; ++j)
//			{
//				log_file << force_log[i][j] << ",";
//			}
//
//			
//
//			log_file << i << "\n";
//		}
//	}
//	std::cout << "Writing to file completed!" << std::endl;
//}


// LOGGING statements end