////////////////////////////////////////////////////////////////////////////////
/// @brief collections
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2010-2011 triagens GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Dr. Frank Celler
/// @author Copyright 2011, triagens GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAGENS_DURHAM_VOC_BASE_COLLECTION_H
#define TRIAGENS_DURHAM_VOC_BASE_COLLECTION_H 1

#include <BasicsC/common.h>

#include <BasicsC/vector.h>

#include <VocBase/datafile.h>
#include <VocBase/vocbase.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
/// @page DurhamCollections Collections
///
/// Data is stored in datafiles. A set of datafiles forms a collection. A
/// datafile can be read-only and sealed or read-write. All datafiles of a
/// collection are stored in a directory. This directory contains the following
/// files:
///
/// - parameter.json: The parameters of a collection.
///
/// - datafile-NNN.db: A read-only datafile. The number NNN is the datafile
///     identifier, see @ref TRI_datafile_t.
///
/// - journal-NNN.db: A read-write datafile used as journal. All new entries
///     of a collection are appended to a journal. The number NNN is the
///     datafile identifier, see @ref TRI_datafile_t.
///
/// - index-NNN.json: An index description. The number NNN is the index
///     identifier, see @ref TRI_index_t.
///
/// The structure @ref TRI_collection_t is abstract. Currently, there are
/// two concrete sub-classes @ref TRI_document_collection_t and
/// @ref TRI_shape_collection_t.
///
/// @section ShapeCollection Shape Collection
///
/// @copydetails TRI_shape_collection_t
///
/// @section PrimaryCollection Document Collection
///
/// @copydetails TRI_primary_collection_t
///
/// @section DocumentCollection Simple Document Collection
///
/// @copydetails TRI_document_collection_t
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                  public constants
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief collection version
////////////////////////////////////////////////////////////////////////////////

#define TRI_COL_VERSION         (2)

////////////////////////////////////////////////////////////////////////////////
/// @brief collection meta info filename
////////////////////////////////////////////////////////////////////////////////

#define TRI_COL_PARAMETER_FILE  "parameter.json"

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                     public macros
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief return whether the collection is a document collection
////////////////////////////////////////////////////////////////////////////////

#define TRI_IS_DOCUMENT_COLLECTION(type) \
  ((type) == TRI_COL_TYPE_DOCUMENT || (type) == TRI_COL_TYPE_EDGE)

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                      public types
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief collection file structure
////////////////////////////////////////////////////////////////////////////////

typedef struct TRI_col_file_structure_s {
  TRI_vector_string_t _journals;
  TRI_vector_string_t _compactors;
  TRI_vector_string_t _datafiles;
  TRI_vector_string_t _indexes;
}
TRI_col_file_structure_t;

////////////////////////////////////////////////////////////////////////////////
/// @brief state of the datafile
////////////////////////////////////////////////////////////////////////////////

typedef enum {
  TRI_COL_STATE_CLOSED = 1,        // collection is closed
  TRI_COL_STATE_READ = 2,          // collection is opened read only
  TRI_COL_STATE_WRITE = 3,         // collection is opened read/append
  TRI_COL_STATE_OPEN_ERROR = 4,    // an error has occurred while opening
  TRI_COL_STATE_WRITE_ERROR = 5    // an error has occurred while writing
}
TRI_col_state_e;

////////////////////////////////////////////////////////////////////////////////
/// @brief collection version
////////////////////////////////////////////////////////////////////////////////

typedef uint32_t TRI_col_version_t;

////////////////////////////////////////////////////////////////////////////////
/// @brief collection enum
////////////////////////////////////////////////////////////////////////////////

typedef enum {
  TRI_COL_TYPE_SHAPE = 1,
  TRI_COL_TYPE_DOCUMENT = 2,
  TRI_COL_TYPE_EDGE = 3
}
TRI_col_type_e;

////////////////////////////////////////////////////////////////////////////////
/// @brief document datafile header marker
////////////////////////////////////////////////////////////////////////////////

typedef struct TRI_col_header_marker_s {
  TRI_df_marker_t base;			// 24 bytes

  TRI_col_type_t _type;                 //  4 bytes

#ifdef TRI_PADDING_32
  char _padding_col_header_marker[4];
#endif

  TRI_voc_cid_t _cid;                   //  8 bytes
}
TRI_col_header_marker_t;

////////////////////////////////////////////////////////////////////////////////
/// @brief collection parameter for create
////////////////////////////////////////////////////////////////////////////////

typedef struct TRI_col_parameter_s {
  TRI_col_type_t _type;              // collection type

  char _name[TRI_COL_PATH_LENGTH];   // name of the collection
  TRI_voc_size_t _maximalSize;       // maximal size of memory mapped file

  bool _waitForSync;                 // if true, wait for msync

  bool _isSystem;                    // if true, this is a system collection
}
TRI_col_parameter_t;

////////////////////////////////////////////////////////////////////////////////
/// @brief collection info block saved to disk as json
////////////////////////////////////////////////////////////////////////////////

typedef struct TRI_col_info_s {
  TRI_col_version_t _version;        // collection version
  TRI_col_type_e _type;              // collection type
  TRI_voc_cid_t _cid;                // collection identifier

  char _name[TRI_COL_PATH_LENGTH];   // name of the collection
  TRI_voc_size_t _maximalSize;       // maximal size of memory mapped file
  bool _waitForSync;                 // if true, wait for msync

  bool _deleted;                     // if true, collections has been deleted
}
TRI_col_info_t;

////////////////////////////////////////////////////////////////////////////////
/// @brief collection
////////////////////////////////////////////////////////////////////////////////

typedef struct TRI_collection_s {
  TRI_col_version_t _version;        // collection version, will be set
  TRI_col_type_e _type;              // collection type, will be set
  TRI_vocbase_t* _vocbase;

  TRI_col_state_e _state;            // state of the collection
  int _lastError;                    // last (critical) error

  TRI_voc_cid_t _cid;                // collection identifier, will we generated
  char _name[TRI_COL_PATH_LENGTH];   // name of the collection

  TRI_voc_size_t _maximalSize;       // maximal size of memory mapped file
  bool _waitForSync;                 // if true, wait for msync

  TRI_voc_size_t _maximumMarkerSize; // largest marker

  bool _deleted;                     // if true, collections has been deleted

  char* _directory;                  // directory of the collection

  TRI_vector_pointer_t _datafiles;   // all datafiles
  TRI_vector_pointer_t _journals;    // all journals
  TRI_vector_pointer_t _compactors;  // all compactor files
  TRI_vector_string_t _indexFiles;   // all index filenames
}
TRI_collection_t;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief initializes a collection parameter block
////////////////////////////////////////////////////////////////////////////////

void TRI_InitParameterCollection (TRI_vocbase_t* vocbase,
                                  TRI_col_parameter_t*,
                                  char const* name,
                                  TRI_col_type_e type,
                                  TRI_voc_size_t maximalSize);

////////////////////////////////////////////////////////////////////////////////
/// @brief creates a new collection
////////////////////////////////////////////////////////////////////////////////

TRI_collection_t* TRI_CreateCollection (TRI_vocbase_t*,
                                        TRI_collection_t*,
                                        char const* path,
                                        TRI_col_info_t* parameter);

////////////////////////////////////////////////////////////////////////////////
/// @brief frees the memory allocated, but does not free the pointer
///
/// Note that the collection must be closed first.
////////////////////////////////////////////////////////////////////////////////

void TRI_DestroyCollection (TRI_collection_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief frees the memory allocated and frees the pointer
////////////////////////////////////////////////////////////////////////////////

void TRI_FreeCollection (TRI_collection_t*);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief creates a parameter info block from file
////////////////////////////////////////////////////////////////////////////////

int TRI_LoadParameterInfoCollection (char const* filename, TRI_col_info_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief saves a parameter info block to file
////////////////////////////////////////////////////////////////////////////////

int TRI_SaveParameterInfoCollection (char const* filename, TRI_col_info_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief updates the parameter info block
////////////////////////////////////////////////////////////////////////////////

int TRI_UpdateParameterInfoCollection (TRI_vocbase_t*,
                                       TRI_collection_t*, 
                                       TRI_col_parameter_t const*);

////////////////////////////////////////////////////////////////////////////////
/// @brief renames a collection
////////////////////////////////////////////////////////////////////////////////

int TRI_RenameCollection (TRI_collection_t* collection, char const* name);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                               protected functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief iterates over a collection
////////////////////////////////////////////////////////////////////////////////

bool TRI_IterateCollection (TRI_collection_t*,
                            bool (*iterator)(TRI_df_marker_t const*, void*, TRI_datafile_t*, bool),
                            void* data);

////////////////////////////////////////////////////////////////////////////////
/// @brief iterates over all index files of a collection
////////////////////////////////////////////////////////////////////////////////

void TRI_IterateIndexCollection (TRI_collection_t* collection,
                                 bool (*iterator)(char const* filename, void*),
                                 void* data);

////////////////////////////////////////////////////////////////////////////////
/// @brief opens an existing collection
////////////////////////////////////////////////////////////////////////////////

TRI_collection_t* TRI_OpenCollection (TRI_vocbase_t*,
                                      TRI_collection_t*,
                                      char const* path);

////////////////////////////////////////////////////////////////////////////////
/// @brief closes an open collection
////////////////////////////////////////////////////////////////////////////////

int TRI_CloseCollection (TRI_collection_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief returns information about the collection files
///
/// Note that the collection must not be loaded
////////////////////////////////////////////////////////////////////////////////

TRI_col_file_structure_t TRI_FileStructureCollectionDirectory (char const* path);

////////////////////////////////////////////////////////////////////////////////
/// @brief frees the information
////////////////////////////////////////////////////////////////////////////////

void TRI_DestroyFileStructureCollection (TRI_col_file_structure_t*);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|// --SECTION--\\|/// @\\}\\)"
// End:
