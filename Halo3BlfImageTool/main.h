#pragma once

struct s_blf_header
{
	long chunk_type;
	long chunk_size;
	short major_version;
	short minor_version;
};
static_assert(sizeof(s_blf_header) == 0xC);

struct s_blf_chunk_start_of_file : s_blf_header
{
	static long const k_chunk_type = '_blf';
	static long const k_version_major = 1;
	static long const k_version_minor = 2;

	// BOM: https://en.wikipedia.org/wiki/Byte_order_mark
	// BOM: { UTF-16 (BE): FE FF }
	unsigned short byte_order_mark;

	char name[32];

	unsigned char pad[2];
};
static_assert(sizeof(s_blf_chunk_start_of_file) == sizeof(s_blf_header) + 0x24);

enum e_map_image_type : unsigned char
{
	_map_image_type_jpg = 0,
	_map_image_type_png,

	k_map_image_type_count
};

struct s_blf_chunk_map_image : s_blf_header
{
public:
	static long const k_chunk_type = 'mapi';
	static long const k_version_major = 1;
	static long const k_version_minor = 1;

	e_map_image_type type;
	long buffer_size;

#pragma warning(push)
#pragma warning(disable : 4200)
	char buffer[];
#pragma warning(pop)
};
static_assert(sizeof(s_blf_chunk_map_image) == sizeof(s_blf_header) + 0x8);
