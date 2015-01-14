#include <stdio.h>
#include <poppler.h>

static void printInfo(gchar *key)
{
  printf("InfoBegin\n");
  printf("InfoKey: %s\n",key);
  printf("InfoValue: ");
}

static void
walkIndex (PopplerDocument *doc,PopplerIndexIter *iter,int level)
{
  do
    {
      int page_num = 0;
      PopplerAction * action;
      PopplerDest * dest;
      gchar *title = NULL;
      PopplerIndexIter *child = poppler_index_iter_get_child (iter);

      action = poppler_index_iter_get_action(iter);
      if (action->type == POPPLER_ACTION_GOTO_DEST) {
	title=g_strdup (action->any.title);
      }

      //get page number of current node
      if (action->goto_dest.dest->type == POPPLER_DEST_NAMED) {
	dest = poppler_document_find_dest (doc, action->goto_dest.dest->named_dest);
	page_num = dest->page_num;
	poppler_dest_free(dest);
      } else {
	page_num = action->goto_dest.dest->page_num;
      }
      printf("BookmarkBegin\n");
      printf("BookmarkTitle: %s\n",title);
      printf("BookmarkLevel: %d\n",level);
      printf("BookmarkPageNumber: %d\n",page_num);

      //free action, as we don't need it anymore
      poppler_action_free(action);

      if (child) {
        walkIndex (doc,child,level+1);
      }
      poppler_index_iter_free (child);
    }
  while (poppler_index_iter_next (iter));
}

int main(int argc, char* argv[])
{
  GError* err = NULL;
  const char *filename;
  gchar *absolute, *uri;
  PopplerDocument* doc;

  PopplerIndexIter* rootIter;

  filename=argv[1];

  if(!filename)
    return 1;

  if (g_path_is_absolute(filename)) {
    absolute = g_strdup (filename);
  } else {
    gchar *dir = g_get_current_dir ();
    absolute = g_build_filename (dir, filename, (gchar *) 0);
    g_free (dir);
  }

  uri = g_filename_to_uri (absolute, NULL, &err);
  g_free (absolute);

  doc = poppler_document_new_from_file(uri, NULL, &err);
  if (!doc) {
    return 1;
  }

  printInfo("Creator");
  printf("%s\n",poppler_document_get_creator(doc));
  printInfo("Title");
  printf("%s\n",poppler_document_get_title(doc));
  printInfo("Producer");
  printf("%s\n",poppler_document_get_producer(doc));
  printInfo("Author");
  printf("%s\n",poppler_document_get_author(doc));

  int pages = poppler_document_get_n_pages(doc);
  printf("NumberOfPages: %d\n",pages);

  rootIter=poppler_index_iter_new(doc);
  walkIndex (doc,rootIter, 1);
  poppler_index_iter_free (rootIter);
  
  g_object_unref (doc);
 	
  return 0;     
}
