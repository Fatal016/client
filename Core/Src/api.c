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

int test(void);

int test(void)
{
	int result;
	CURL *curl = curl_easy_init();
	CURLcode res;
	sqlite3 *db;
	
	Result r, r_sc;

	struct memory chunk = {0};
	char *sql = malloc(SQL_QUERY_MAX_LENGTH * sizeof(char));
	result = sqlite3_open("test.db", &db);
	
	struct query_params *query = (struct query_params*)malloc(sizeof(struct query_params));
	query->query = "Alien";
	query->language = "en";
	query->primary_release_year = 0;
	query->page = 1;
	query->region = NULL;
	query->year = 0;
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

	r = manage_results_cache_table(
		db,
		sql,
		&(State){CREATE},
		NULL
	);

	r = manage_results_cache_table(
		db,
		sql,
		&(State){CREATE},
		NULL
	);

	r_sc = manage_search_cache_table(
		db,
		sql,
		&(State){UPDATE},
		query
	);
	response->search_id = *(int*)r_sc.data;


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


		cJSON_handler = cJSON_GetObjectItemCaseSensitive(
			api_result,
			"backdrop_path"
		);
		response->results[response->result_index]->backdrop_path = cJSON_handler->valuestring;

/*		
		cJSON_handler = cJSON_GetObjectItemCaseSensitive(
			api_result,
			"genre_ids"
		);
*/
		// response ...

		cJSON_handler = cJSON_GetObjectItemCaseSensitive(
			api_result,
			"id"
		);
		response->results[response->result_index]->id = cJSON_handler->valueint;


		cJSON_handler = cJSON_GetObjectItemCaseSensitive(
			api_result,
			"original_language"
		);
		response->results[response->result_index]->original_language = cJSON_handler->valuestring;

		cJSON_handler = cJSON_GetObjectItemCaseSensitive(
			api_result,
			"original_title"
		);
		response->results[response->result_index]->original_title = cJSON_handler->valuestring;


		cJSON_handler = cJSON_GetObjectItemCaseSensitive(
			api_result,
			"overview"
		);
		response->results[response->result_index]->overview = sanitize_input(cJSON_handler->valuestring);
		

	
		cJSON_handler = cJSON_GetObjectItemCaseSensitive(
			api_result,
			"popularity"
		);
		response->results[response->result_index]->popularity = cJSON_handler->valuedouble;

		cJSON_handler = cJSON_GetObjectItemCaseSensitive(
			api_result,
			"poster_path"
		);
		response->results[response->result_index]->poster_path = cJSON_handler->valuestring;


		cJSON_handler = cJSON_GetObjectItemCaseSensitive(
			api_result,
			"release_date"
		);
		response->results[response->result_index]->release_date = cJSON_handler->valuestring;

		cJSON_handler = cJSON_GetObjectItemCaseSensitive(
			api_result,
			"title"
		);
		response->results[response->result_index]->title = cJSON_handler->valuestring;


		cJSON_handler = cJSON_GetObjectItemCaseSensitive(
			api_result,
			"poster_path"
		);
		response->results[response->result_index]->poster_path = cJSON_handler->valuestring;

		cJSON_handler = cJSON_GetObjectItemCaseSensitive(
			api_result,
			"vote_average"
		);
		response->results[response->result_index]->vote_average = cJSON_handler->valuedouble;

		cJSON_handler = cJSON_GetObjectItemCaseSensitive(
			api_result,
			"vote_count"
		);
		response->results[response->result_index]->vote_count = cJSON_handler->valueint;



		r = manage_results_cache_table(
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

	
	return result;
}
