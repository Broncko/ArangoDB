////////////////////////////////////////////////////////////////////////////////
/// @brief edge collection
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

#ifndef TRIAGENS_DURHAM_VOC_BASE_EDGE_COLLECTION_H
#define TRIAGENS_DURHAM_VOC_BASE_EDGE_COLLECTION_H 1

#include <VocBase/document-collection.h>

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                   EDGE COLLECTION
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// --SECTION--                                                      public types
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief edge from and to
////////////////////////////////////////////////////////////////////////////////

typedef struct TRI_document_edge_s {
  TRI_voc_cid_t _fromCid;
  TRI_voc_did_t _fromDid;

  TRI_voc_cid_t _toCid;
  TRI_voc_did_t _toDid;
}
TRI_document_edge_t;

////////////////////////////////////////////////////////////////////////////////
/// @brief edge direction
////////////////////////////////////////////////////////////////////////////////

typedef enum {
  TRI_EDGE_UNUSED = 0,
  TRI_EDGE_IN = 1,
  TRI_EDGE_OUT = 2,
  TRI_EDGE_ANY = 3
}
TRI_edge_direction_e;

////////////////////////////////////////////////////////////////////////////////
/// @brief index entry for edges
////////////////////////////////////////////////////////////////////////////////

typedef struct TRI_edge_header_s {
  TRI_doc_mptr_t const* _mptr;
  TRI_edge_direction_e _direction;
  TRI_voc_cid_t _cid; // from or to, depending on the direction
  TRI_voc_did_t _did; // from or to, depending on the direction
}
TRI_edge_header_t;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                       EDGES INDEX
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief initialise the edges index of a collection
////////////////////////////////////////////////////////////////////////////////

void TRI_InitEdgesDocumentCollection (TRI_document_collection_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief free all edges in the collection's edges index
////////////////////////////////////////////////////////////////////////////////

void TRI_FreeEdgesDocumentCollection (TRI_document_collection_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief insert an edge into the edges index
////////////////////////////////////////////////////////////////////////////////
  
int TRI_InsertEdgeDocumentCollection (TRI_document_collection_t*,
                                      TRI_doc_mptr_t const*);

////////////////////////////////////////////////////////////////////////////////
/// @brief remove an edge from the edges index
////////////////////////////////////////////////////////////////////////////////
  
void TRI_DeleteEdgeDocumentCollection (TRI_document_collection_t*,
                                       TRI_doc_mptr_t const*);

////////////////////////////////////////////////////////////////////////////////
/// @brief looks up edges
////////////////////////////////////////////////////////////////////////////////

TRI_vector_pointer_t TRI_LookupEdgesDocumentCollection (TRI_document_collection_t* edges,
                                                        TRI_edge_direction_e direction,
                                                        TRI_voc_cid_t cid,
                                                        TRI_voc_did_t did);

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
