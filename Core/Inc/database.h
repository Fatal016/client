#ifndef H_DATABASE
#define H_DATABASE

#include <sqlite3.h>
#include <stdint.h>

//#include "server.h"
//#include "account.h"

#define SEARCH_QUERY_MAX_LENGTH 128
#define SQL_QUERY_MAX_LENGTH 2048

typedef enum {
        CREATE,
        UPDATE,
        DESTROY
} State;

typedef struct {
	unsigned int rc;
	char* msg;
	void* data;
} Result;

Result manage_search_cache_table(sqlite3*, char*, State*, const void*);
Result create_search_cache_table(sqlite3*, char*);
Result update_search_cache_table(sqlite3*, char*, const void*);
Result destroy_search_cache_table(sqlite3*, char*);

Result manage_results_cache_table(sqlite3*, char*, State*, const void*);
Result create_results_cache_table(sqlite3*, char*);
Result update_results_cache_table(sqlite3*, char*, const void*);

char* sanitize_input(const char*);


struct query_result {
	/*
	adult
	video
	*/
	char* backdrop_path;
	int* genre_ids;
	int id;
	char* original_language;
	char* original_title;
	char* overview;
	double popularity;
	char* poster_path;
	char* release_date;
	char* title;
	double vote_average;
	int vote_count;
	uint8_t* query_flags;
};

struct query_params {
	char* query;
	char* language;
	int primary_release_year;
	uint32_t page;
	char* region;
	int year;

	/*
	bit 0 -> include_adult
	bit 1
	bit 2
	bit 3
	bit 4
	bit 5
	bit 6
	bit 7
	*/
	uint8_t* query_flags;
};

struct query_response {
	uint32_t page;
	uint32_t total_pages;
	uint32_t total_results;
	uint32_t result_index;
	int search_id;
	struct query_params* params;
	struct query_result** results;
};



//int create_sensor_table(sqlite3*, char*, struct sensor_info*);
//int create_sensor_entry(sqlite3*, char*, struct sensor_info*);
//int update_sensor_table(struct sensor_info*);
//int query_sensor_table(struct sensor_info*, char*);

//int create_account_table(sqlite3*, char*);
//int create_account_entry(sqlite3*, char*, struct user_info*);
//int update_account_table(struct user_info*);
//int query_account_table(struct sensor_info*, char*, char*);

int callback(void*, int, char**, char**);

#endif
