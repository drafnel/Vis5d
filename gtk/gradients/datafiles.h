/***** Types *****/

typedef enum
{
  INCLUDE_TEMP_DIR = 1 << 0,
  MODE_EXECUTABLE  = 1 << 1
} GimpDataFileFlags;

typedef void (* GimpDataFileLoaderFunc) (gchar *filename);


/***** Functions *****/

void datafiles_read_directories (gchar                  *path_str,
				 GimpDataFileLoaderFunc  loader_func,
				 GimpDataFileFlags       flags);

