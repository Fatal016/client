#ifndef H_DATABASE
#define H_DATABASE

#include <sqlite3.h>
#include <stdint.h>

//#include "server.h"
//#include "account.h"

#define SEARCH_QUERY_MAX_LENGTH 128
#define SQL_QUERY_MAX_LENGTH 256

typedef enum {
        CREATE,
        UPDATE,
        DESTROY
} State;

int manage_search_cache_table(sqlite3*, char*, State*, const void*);
int create_search_cache_table(sqlite3*, char*);
int update_search_cache_table(sqlite3*, char*, const void*);
int destroy_search_cache_table(sqlite3*, char*);

int manage_results_cache_table(sqlite3*, char*, State*, const void*);
int create_results_cache_table(sqlite3*, char*);
int update_results_cache_table(sqlite3*, char*, const void*);

char* sanitize_input(const char*);

struct query_result {
	/*
	adult
	video
	*/
	char* backdrop_path;
	int32_t* genre_ids;
	int32_t id;
	char* original_language;
	char* original_title;
	char* overview;
	float popularity;
	char* poster_path;
	char* release_date;
	char* title;
	float vote_average;
	int32_t vote_count;
	uint8_t* query_flags;
};

struct query_params {
	char* query;
	char* language;
	char* primary_release_year;
	uint32_t page;
	char* region;
	char* year;

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
