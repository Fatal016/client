#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "../Inc/database.h"

struct Result manage_search_cache_table(sqlite3 *db, char *sql, State *state, const void *data)
{	
	struct Result r;
/*
	if (state == NULL) {
		r.rc = NULL_STATE;
		r.msg = "State: NULL. ";
		return r;
	}
*/
	switch(*state) {
		case CREATE:
			r = create_search_cache_table(db, sql);
			break;
		case UPDATE:
			r = create_search_cache_table(db, sql);
			r = update_search_cache_table(db, sql, data);
			break;
		case DESTROY:
			r = destroy_search_cache_table(db, sql);
			break;
	}

	return r;
}

struct Result manage_results_cache_table(sqlite3 *db, char *sql, State *state, const void *data)
{	
	struct Result r;
//	char debug_buffer[256];
//	char *zErrMsg;

//	if (state == NULL) return -1;

	switch(*state) {
		case CREATE:
			r = create_results_cache_table(db, sql);
			break;
		case UPDATE:
			r = create_results_cache_table(db, sql);
			r = update_results_cache_table(db, sql, data);
			break;
		case DESTROY:
			break;
	}

	r.rc = 0;
	return r;
}

struct Result create_search_cache_table(sqlite3 *db, char *sql) {

	struct Result r;	
	int result;
	char *zErrMsg;

	// Need to add error handling
	strncpy(
		sql, 
		"CREATE TABLE SEARCH("      		\
		"search_id INTEGER PRIMARY KEY,"	\
		"query TEXT NOT NULL,"			\
		"language TEXT,"			\
		"primary_release_year INTEGER,"		\
		"region TEXT,"				\
		"year INTEGER,"				\
		"adult INTEGER);"
		, SQL_QUERY_MAX_LENGTH
	);

	result = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (result != SQLITE_OK) {}

	r.rc = 0;
	return r;
}
/*
struct Result query_search_cache_table(sqlite3 *db, char *sql, const void *data)
{
	struct Result r;
data;

	result = snprintf(sql, SQL_QUERY_MAX_LENGTH,
		""
	)
}
*/
struct Result update_search_cache_table(sqlite3 *db, char *sql, const void *data)
{
	struct Result r;
	int result;
	char *zErrMsg;
	sqlite3_stmt *stmt;

	struct query_params *query = (struct query_params*)data;
	
	result = snprintf(sql, SQL_QUERY_MAX_LENGTH,
		"INSERT INTO SEARCH ("		\
			"query,"		\
			"language,"		\
			"primary_release_year,"	\
			"region,"		\
			"year,"			\
			"adult"			\
		")"				\
		"VALUES("			\
			"'%s',"			\
			"'%s',"			\
			"'%d',"			\
			"'%s',"			\
			"'%d',"			\
			"'%d'"			\
			");",
		query->query,
		query->language,
		query->primary_release_year,
		query->region,
		query->year,
		0 // Will need to update
	);
//	if (result < 0) {
//		r.rc = EXIT_FAILURE;
//		r.msg = strerror(errno);
//		return r;
//	} else if (result >= sizeof(SQL_QUERY_MAX_LENGTH)) {
//		r.rc = EXIT_FAILURE;
//		r.msg = strerror(errno);
//		return r;
//	}

	result = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (result != SQLITE_OK) {
		r.rc = result;
		// Need to figure out sqlite_free for this and where to put
		r.msg = zErrMsg;
		return r;
	}	

	strncpy(
		sql, 
		"SELECT last_insert_rowid();"
		, SQL_QUERY_MAX_LENGTH
	);
//	if (result < 0) {
//		r.rc = EXIT_FAILURE;
//		r.msg = strerror(errno);
//		return r;
//	} else if (result >= sizeof(SQL_QUERY_MAX_LENGTH)) {
//		r.rc = EXIT_FAILURE;
//		r.msg = strerror(errno);
//		return r;
//	}

	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		r.rc = result;
		// Need to figure out sqlite_free for this and where to put
		r.msg = zErrMsg;
		return r;
	}	

	result = sqlite3_step(stmt);

//	if (result != SQLITE_OK) {
//		r.rc = result;
		// Need to figure out sqlite_free for this and where to put
//		r.msg = zErrMsg;
//		return r;
//	}	

	result = sqlite3_column_int(stmt, 0);
	printf("struct Result: %d\n", result);

	r.rc = 0;
	r.data = (void*)malloc(sizeof(int));
	*(int*)(r.data) = result;

	return r;
}

struct Result create_results_cache_table(sqlite3 *db, char *sql)
{
	struct Result r;
	int result;
//	char debug_buffer[256];
	char *zErrMsg;

	strncpy(
		sql,
		"CREATE TABLE RESULTS("      		\
		"result_id INTEGER PRIMARY KEY,"	\
		"adult INTEGER,"			\
		"backdrop_path TEXT,"			\
		"genre_ids TEXT,"			\
		"id INTEGER,"				\
		"original_language TEXT,"		\
		"original_title TEXT,"			\
		"overview TEXT,"			\
		"popularity REAL,"			\
		"poster_path TEXT,"			\
		"release_date TEXT,"			\
		"title TEXT,"				\
		"video INTEGER,"			\
		"vote_average REAL,"			\
		"vote_count INTEGER,"			\
		"search_id INTEGER,"			\
		"FOREIGN KEY(search_id) "		\
		"REFERENCES SEARCH(search_id) "		\
		"ON DELETE CASCADE);"
		, SQL_QUERY_MAX_LENGTH
	);

	result = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (result != SQLITE_OK) {
//		r.rc = result;
		// Need to figure out sqlite_free for this and where to put
//		r.msg = zErrMsg;
//		return r;
	}	

	r.rc = 0;
	return r;
}

// Cache is for like past searches or something similar
// Short cache time


struct Result update_results_cache_table(sqlite3 *db, char *sql, const void *data)
{
	struct Result r;
	int rc;
	char *zErrMsg;

	struct query_response *response = (struct query_response*)data;
	struct query_result *result = response->results[response->result_index];

	snprintf(sql, SQL_QUERY_MAX_LENGTH,
		"INSERT INTO RESULTS ("		\
			"adult,"		\
			"backdrop_path,"	\
			"genre_ids,"		\
			"id,"			\
			"original_language,"	\
			"original_title,"	\
			"overview,"		\
			"popularity,"		\
			"poster_path,"		\
			"release_date,"		\
			"title,"		\
			"video,"		\
			"vote_average,"		\
			"vote_count,"		\
			"search_id"		\
		") "				\
		"VALUES("			\
			"'%d',"			\
			"'%s',"			\
			"'%s',"			\
			"'%d',"			\
			"'%s',"			\
			"'%s',"			\
			"'%s',"			\
			"'%f',"			\
			"'%s',"			\
			"'%s',"			\
			"'%s',"			\
			"'%d',"			\
			"'%f',"			\
			"'%d',"			\
			"'%d'"			\
		");",
		0,
	/*	result->adult, */
		result->backdrop_path,
		"NULL",
	/* 	result->genre_ids, // Need to comma separate or similar */
		result->id,
		result->original_language,
		result->original_title,
		result->overview,
		result->popularity,
		result->poster_path,
		result->release_date,
		result->title,
		0,
	/*	result->video, */
		result->vote_average,
		result->vote_count,
		response->search_id
	);

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		r.rc = rc;
		// Need to figure out sqlite_free for this and where to put
		r.msg = zErrMsg;
		return r;
	}	

//	printf("SQL error: %s on %s\n", zErrMsg, (char *)data);

	r.rc = 0;
	return r;
}

struct Result destroy_search_cache_table(sqlite3* db, char *sql)
{
	struct Result r;
	int result;
//	char debug_buffer[256];
	char *zErrMsg;

	strncpy(sql,
		"DROP TABLE IF EXISTS SEARCH;",
		SQL_QUERY_MAX_LENGTH
	);

	result = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (result != SQLITE_OK) {}

	r.rc = 0;
	return r;
}

char* sanitize_input(const char* input) {

//	int result;

	size_t len = strlen(input);
	size_t extra = 0;

	for (size_t i = 0; i < len; i++) {
		if (input[i] == '\'') extra++;
	}

	char* output = malloc(len + extra + 1);

	size_t j = 0;
	for (size_t i = 0; i < len; i++) {
		if (input[i] == '\'') {
			output[j++] = '\'';
			output[j++] = '\'';
		} else {
			output[j++] = input[i];
		}
	}

	output[j] = '\0';
	return output;
}


int callback(void *data, int argc, char **argv, char **azColName) {

	int i = 0;

	strcpy((char *)data, argv[i] ? argv[i] : "NULL");

	return 0;
}
