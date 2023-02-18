#include "main.h"

#include <Windows.h>
#include <assert.h>
#include <stdio.h>

int extract_image_data(char const* in_file_path, DWORD in_file_size);

int main(int argc, char const* argv[])
{
	DWORD result = 0;

	if (argc != 2)
	{
		printf("Halo3BlfImageTool.exe \"C:\\Games\\Halo3\\maps\\images\"");

		return (-1);
	}

	char const* images_dir = argv[1];

	WIN32_FIND_DATAA find_data;
	LARGE_INTEGER filesize;
	char find_search_path[MAX_PATH];
	char find_file_path[MAX_PATH];
	HANDLE find_handle = INVALID_HANDLE_VALUE;

	if (strlen(images_dir) > (MAX_PATH - 3))
		return (-2);

	sprintf_s(find_search_path, MAX_PATH, "%s\\*.blf", images_dir);

	find_handle = FindFirstFileA(find_search_path, &find_data);
	if (find_handle == INVALID_HANDLE_VALUE)
		return result;

	do
	{
		if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			filesize.LowPart = find_data.nFileSizeLow;
			filesize.HighPart = find_data.nFileSizeHigh;

			sprintf_s(find_file_path, MAX_PATH, "%s\\%s", images_dir, find_data.cFileName);

			extract_image_data(find_file_path, (DWORD)filesize.QuadPart);
		}
	} while (FindNextFileA(find_handle, &find_data) != 0);

	result = GetLastError();
	assert(result == ERROR_NO_MORE_FILES);

	FindClose(find_handle);

	return result;
}

int extract_image_data(char const* in_file_path, DWORD in_file_size)
{
	printf("extracting %s\n", in_file_path);

	char* out_file_path = _strdup(in_file_path);

	int string_index = (int)strlen(out_file_path);
	while (string_index != 0 && out_file_path[string_index - 1] != '.')
		string_index--;

	char* out_file_ext = out_file_path + string_index;

	HANDLE in_file_handle = INVALID_HANDLE_VALUE;
	DWORD in_bytes_read = 0;
	void* in_file_buffer = NULL;

	HANDLE out_file_handle = INVALID_HANDLE_VALUE;
	DWORD out_file_size = 0;
	DWORD out_bytes_written = 0;
	CHAR* out_file_buffer = NULL;

	s_blf_chunk_start_of_file* start_of_file_chunk = NULL;
	s_blf_chunk_map_image* map_image_chunk = NULL;

	in_file_handle = CreateFileA(in_file_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (in_file_handle == INVALID_HANDLE_VALUE)
		return (-10);

	in_file_buffer = malloc(in_file_size);

	if (in_file_buffer == NULL)
		return (-11);

	if (ReadFile(in_file_handle, in_file_buffer, in_file_size, &in_bytes_read, NULL))
		assert(in_file_size == in_bytes_read);

	CloseHandle(in_file_handle);

	start_of_file_chunk = (s_blf_chunk_start_of_file*)in_file_buffer;
	map_image_chunk = (s_blf_chunk_map_image*)(start_of_file_chunk + 1);

	switch (map_image_chunk->type)
	{
	case _map_image_type_jpg:
		strcpy_s(out_file_ext, 4, "jpg");
		break;
	case _map_image_type_png:
		strcpy_s(out_file_ext, 4, "png");
		break;
	}

	out_file_handle = CreateFileA(out_file_path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (out_file_handle == INVALID_HANDLE_VALUE)
	{
		free(in_file_buffer);
		return (-12);
	}

	out_file_size = map_image_chunk->buffer_size;
	out_file_buffer = map_image_chunk->buffer;

	if (start_of_file_chunk->byte_order_mark == 0xFEFF)
		out_file_size = _byteswap_ulong(out_file_size);

	if (WriteFile(out_file_handle, out_file_buffer, out_file_size, &out_bytes_written, NULL))
		assert(out_file_size == out_bytes_written);

	CloseHandle(out_file_handle);
	free(in_file_buffer);

	return 0;
}

