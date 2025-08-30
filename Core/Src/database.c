#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../Inc/database.h"

int create_search_cache_table(sqlite3 *db, char *sql) {
	
	int result;
//	char debug_buffer[256];
	char *zErrMsg;

	// Need to add error handling
	strncpy(
		sql, 
		"CREATE TABLE SEARCH("      	\
		"search_id INT PRIMARY KEY,"	\
		"search_string TEXT NOT NULL);"	\
		, SQL_QUERY_MAX_LENGTH
	);

	result = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	return 0;
}

int create_results_cache_table(sqlite3 *db, char *sql)
{
	int result;
//	char debug_buffer[256];
	char *zErrMsg;

	strncpy(
		sql,
		"CREATE TABLE RESULTS("      		\
		"result_id INT PRIMARY KEY,"		\
		"search_id INT,"			\
		"backdrop_path TEXT,"			\
		"FOREIGN KEY (search_id)"		\
		"REFERENCES SearchStrings(search_id)"	\
		"ON DELETE CASCADE);"
		, SQL_QUERY_MAX_LENGTH
	);

	result = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	return 0;
}

int update_search_cache_table(sqlite3 *db, char *sql, const void *data) {

	int result;
//	char debug_buffer[256];
	char *zErrMsg;
	sqlite3_stmt *stmt;

	struct query_params *query = (struct query_params*)data;

	char *sanitized = sanitize_input(query->query);

	snprintf(sql, SQL_QUERY_MAX_LENGTH,
		"INSERT INTO SEARCH (SEARCH_STRING)"	\
		"VALUES('%s');"
		, sanitized
	);

	result = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	printf("SQL error: %s on %s\n", zErrMsg, (char *)data);


	strncpy(
		sql, 
		"SELECT last_insert_rowid();"
		, SQL_QUERY_MAX_LENGTH
	);

	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	result = sqlite3_step(stmt);

	// getting last row so that I can reference it for foreign key input of
	// data in other table
	printf("Output: %s", (char *)sqlite3_column_text(stmt, 1));

	//printf("result: %s")

	return 0;
}

int update_results_cache_table(sqlite3 *db, char *sql, const void *data)
{
	int result;
//	char debug_buffer[256];
	char *zErrMsg;

	struct query_response *response = (struct query_response*)data;

	char *sanitized = sanitize_input(response->params->query);

	snprintf(sql, SQL_QUERY_MAX_LENGTH,
		"INSERT INTO RESULTS (backdrop_path)"	\
		"VALUES('%s');"
		, sanitized
	);

	result = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	printf("SQL error: %s on %s\n", zErrMsg, (char *)data);

	return 0;
}

int destroy_search_cache_table(sqlite3* db, char *sql) {

	int result;
//	char debug_buffer[256];
	char *zErrMsg;

	strncpy(sql,
		"DROP TABLE IF EXISTS SEARCH;",
		SQL_QUERY_MAX_LENGTH
	);

	result = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	return 0;
}

char* sanitize_input(const char* input) {

	int result;

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

int manage_search_cache_table(sqlite3 *db, char *sql, State *state, const void *data)
{	
	int result;
//	char debug_buffer[256];
	char *zErrMsg;

	if (state == NULL) return -1;

	switch(*state) {
		case CREATE:
			result = create_search_cache_table(db, sql);
			break;
		case UPDATE:
			result = create_search_cache_table(db, sql);
			result = update_search_cache_table(db, sql, data);
			break;
		case DESTROY:
			result = destroy_search_cache_table(db, sql);
			break;
	}

	return 0;
}

int manage_results_cache_table(sqlite3 *db, char *sql, State *state, const void *data)
{	
	int result;
//	char debug_buffer[256];
	char *zErrMsg;

	if (state == NULL) return -1;

	switch(*state) {
		case CREATE:
			result = create_results_cache_table(db, sql);
			break;
		case UPDATE:
			result = create_results_cache_table(db, sql);
			result = update_results_cache_table(db, sql, data);
			break;
	}

	return 0;
}

int callback(void *data, int argc, char **argv, char **azColName) {

	int i = 0;

	strcpy((char *)data, argv[i] ? argv[i] : "NULL");

	return 0;
}
