#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dirent.h>

#include <gtk/gtk.h>

#include "support.h"
#include "datafiles.h"

GList * gimp_path_parse                 (gchar     *path,
					 gint       max_paths,
					 gboolean   check,
					 GList    **check_failed);

static gboolean    filestat_valid = FALSE;
static struct stat filestat;

/**
 * gimp_path_free:
 * @path: A list of directories as returned by gimp_path_parse().
 *
 * This function frees the memory allocated for the list and it's strings.
 **/
void
gimp_path_free (GList *path)
{
  GList *list;

  if (path)
    {
      for (list = path; list; list = g_list_next (list))
	{
	  g_free (list->data);
	}

      g_list_free (path);
    }
}

void
datafiles_read_directories (gchar                  *path_str,
			    GimpDataFileLoaderFunc  loader_func,
			    GimpDataFileFlags       flags)
{
  gchar *local_path;
  GList *path;
  GList *list;
  gchar *filename;
  gint   err;
  DIR   *dir;
  struct dirent *dir_ent;

  if (path_str == NULL)
    return;

  local_path = g_strdup (path_str);

#ifdef __EMX__
  /*
   *  Change drive so opendir works.
   */
  if (local_path[1] == ':')
    {
      _chdrive (local_path[0]);
    }
#endif  

  path = gimp_path_parse (local_path, 16, TRUE, NULL);

  for (list = path; list; list = g_list_next (list))
    {
      /* Open directory */
      dir = opendir ((gchar *) list->data);

      if (!dir)
	{
	  g_message ("error reading datafiles directory \"%s\"",
		     (gchar *) list->data);
	}
      else
	{
	  while ((dir_ent = readdir (dir)))
	    {
	      filename = g_strdup_printf ("%s%s",
					  (gchar *) list->data,
					  dir_ent->d_name);

	      /* Check the file and see that it is not a sub-directory */
	      err = stat (filename, &filestat);

	      if (!err && S_ISREG (filestat.st_mode) &&
		  (!(flags & MODE_EXECUTABLE) ||
		   (filestat.st_mode & S_IXUSR)))
		{
		  filestat_valid = TRUE;
		  (*loader_func) (filename);
		  filestat_valid = FALSE;
		}

	      g_free (filename);
	    }

	  closedir (dir);
	}
    }

  gimp_path_free (path);
  g_free (local_path);
}


/**
 * gimp_path_parse:
 * @path: A list of directories separated by #G_SEARCHPATH_SEPARATOR.
 * @max_paths: The maximum number of directories to return.
 * @check: #TRUE if you want the directories to be checked.
 * @check_failed: Returns a #GList of path elements for which the
 *                check failed. Each list element is guaranteed
 *		  to end with a #G_PATH_SEPARATOR.
 *
 * Returns: A #GList of all directories in @path. Each list element
 *	    is guaranteed to end with a #G_PATH_SEPARATOR.
 **/
GList *
gimp_path_parse (gchar     *path,
		 gint       max_paths,
		 gboolean   check,
		 GList    **check_failed)
{
  gchar  *home;
  gchar **patharray;
  GList  *list = NULL;
  GList  *fail_list = NULL;
  gint    i;

  struct stat filestat;
  gint        err = FALSE;

  if (!path || !*path || max_paths < 1 || max_paths > 256)
    return NULL;

  home = g_get_home_dir ();

  patharray = g_strsplit (path, G_SEARCHPATH_SEPARATOR_S, max_paths);

  for (i = 0; i < max_paths; i++)
    {
      GString *dir;

      if (!patharray[i])
	break;

#ifndef G_OS_WIN32
      if (*patharray[i] == '~')
	{
	  dir = g_string_new (home);
	  g_string_append (dir, patharray[i] + 1);
	}
      else
#endif
	{
	  dir = g_string_new (patharray[i]);
	}

#ifdef __EMX__
      _fnslashify (dir);
#endif

      if (check)
	{
	  /*  check if directory exists  */
	  err = stat (dir->str, &filestat);

	  if (!err && S_ISDIR (filestat.st_mode))
	    {
	      if (dir->str[dir->len - 1] != G_DIR_SEPARATOR)
		g_string_append_c (dir, G_DIR_SEPARATOR);
	    }
	}

      if (!err)
	list = g_list_prepend (list, g_strdup (dir->str));
      else if (check_failed)
	fail_list = g_list_prepend (fail_list, g_strdup (dir->str));

      g_string_free (dir, TRUE);
    }

  g_strfreev (patharray);

  list = g_list_reverse (list);

  if (check && check_failed)
    {
      fail_list = g_list_reverse (fail_list);
      *check_failed = fail_list;
    }

  return list;
}

