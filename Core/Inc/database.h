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

struct Result {
	int rc;
	char* msg;
	void* data;
};

struct Result manage_search_cache_table(sqlite3*, char*, State*, const void*);
struct Result create_search_cache_table(sqlite3*, char*);
struct Result update_search_cache_table(sqlite3*, char*, const void*);
struct Result destroy_search_cache_table(sqlite3*, char*);

struct Result manage_results_cache_table(sqlite3*, char*, State*, const void*);
struct Result create_results_cache_table(sqlite3*, char*);
struct Result update_results_cache_table(sqlite3*, char*, const void*);

struct Result manage_config_table(sqlite3*, char*, State*, const void*);
struct Result create_config_table(sqlite3*, char*);
struct Result update_config_table(sqlite3*, char*, const void*);
struct Result query_config_table(sqlite3*, char*, char*);
struct Result destroy_config_table(sqlite3*, char*);

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



int callback(void*, int, char**, char**);

#endif
