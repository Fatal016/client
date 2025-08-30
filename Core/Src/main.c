#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "../Inc/cJSON.h"
#include "../Inc/database.h"

#include <sqlite3.h>

/* API Params (Would be stored and executed on in AWS API Gateway or similar) */
#define endpoint_hostname "https://api.themoviedb.org"
#define endpoint_token "Authorization: Bearer eyJhbGciOiJIUzI1NiJ9.eyJhdWQiOiJkZDc3YmU2MTQyYjc3ZTNhZGQ5NGM0ZWY1ZTljZWQ0YyIsIm5iZiI6MTY2MTM2NTY1Mi40NzcsInN1YiI6IjYzMDY2ZDk0NmU5MzhhMDA3YmYwZjM3MCIsInNjb3BlcyI6WyJhcGlfcmVhZCJdLCJ2ZXJzaW9uIjoxfQ.LHKyFJPtoVCM9meJ07IDoJsblgqB4ITu3v9qgIvA9Yo"
#define api_path "/3"
#define search_query "/search/movie?query="

struct memory {
	char *response;
	size_t size;
};


static size_t cb(char *data, size_t size, size_t nmemb, void *clientp)
{
	size_t realsize = size * nmemb;
	struct memory *mem = (struct memory *)clientp;

	char *ptr = realloc(mem->response, mem->size + realsize + 1);
	if (!ptr) {
		return 0;
	}

	mem->response = ptr;
	memcpy(&(mem->response[mem->size]), data, realsize);
	mem->size += realsize;
	mem->response[mem->size] = 0;
	
	return realsize;
}


int main(int argc, char** argv)
{
	int result;
	CURL *curl = curl_easy_init();
	CURLcode res;
	sqlite3 *db;
	
	struct memory chunk = {0};
	char *sql = malloc(256 * sizeof(char));
	result = sqlite3_open("test.db", &db);
	
	/* Would by dynamic from TUI interface */
	const char *search_term = "Star Wars";

	struct query_params *query = (struct query_params*)malloc(sizeof(struct query_params));
	query->query = "Star Wars";
	query->language = "en";
	query->primary_release_year = NULL;
	query->page = 1;
	query->region = NULL;
	query->year = NULL;
	query->query_flags = NULL;


	struct query_response *response = (struct query_response*)malloc(sizeof(struct query_response));
	response->result_index = 0;
	response->params = query;

	char *search_url = (char *)malloc(
				sizeof(char) *
				SEARCH_QUERY_MAX_LENGTH
	);

	query->query = curl_easy_escape(
				curl,
				query->query,
				0
	);

	struct curl_slist *list = NULL;
	
	result = manage_results_cache_table(
		db,
		sql,
		&(State){CREATE},
		NULL
	);

	result = manage_results_cache_table(
		db,
		sql,
		&(State){CREATE},
		NULL
	);

	result = manage_search_cache_table(
		db,
		sql,
		&(State){UPDATE},
		query
	);
	

	list = curl_slist_append(list, endpoint_token);
	list = curl_slist_append(list, "Accept: application/json");

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);


	/* Would be executed by Lambda or similar */
	result = snprintf(
			search_url,
			SEARCH_QUERY_MAX_LENGTH,
			"%s%s%s%s&page=%d",
			endpoint_hostname,
			api_path,
			search_query,
			query->query,
			query->page
	);

	curl_easy_setopt(
		curl,
		CURLOPT_URL,
		search_url
	);

	res = curl_easy_perform(curl);

	if (res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
		curl_easy_strerror(res));

	cJSON *cJSON_handler;

	cJSON *api_response = cJSON_Parse(chunk.response);
	printf("%s\n", cJSON_Print(api_response));

	cJSON_handler = cJSON_GetObjectItemCaseSensitive(api_response, "page");
	/* Error handling ... */
	response->page = cJSON_handler->valueint;
	
	cJSON_handler = cJSON_GetObjectItemCaseSensitive(api_response, "total_pages");
	response->total_pages = cJSON_handler->valueint;

	cJSON_handler = cJSON_GetObjectItemCaseSensitive(api_response, "total_results");
	response->total_results = cJSON_handler->valueint;

	/* Allocate memory */
	response->results = (struct query_result**)malloc(sizeof(struct query_result*) * response->total_results);


	const cJSON *api_results = cJSON_GetObjectItemCaseSensitive(api_response, "results");
	const cJSON *api_result = NULL;

	cJSON_ArrayForEach(api_result, api_results)
	{
		response->results[response->result_index] = (struct query_result*)malloc(sizeof(struct query_result));

//		struct query_result *api_response = (struct query_result*)malloc(sizeof(struct query_result));


		cJSON *backdrop_path = cJSON_GetObjectItemCaseSensitive(
			api_result,
			"backdrop_path"
		);
		response->results[response->result_index]->backdrop_path = backdrop_path->valuestring;
	
		printf("%s\n", response->results[response->result_index]->backdrop_path);	

		result = manage_results_cache_table(
			db,
			sql,
			&(State){UPDATE},
			response
		);
//		result = manage_search_table(db, sql, &test, string);
//		printf("%s\n", string);		
		response->result_index++;
	}
/*
	result = manage_search_table(

			db,
			sql,
			&(State){DESTROY},
			NULL
	);

*/

//	cJSON *adult = cJSON_GetObjectItemCaseSensitive(json, "adult");
//    	printf("Adult: %d\n", adult->type);




	curl_easy_cleanup(curl);

	
	return 0;
}
