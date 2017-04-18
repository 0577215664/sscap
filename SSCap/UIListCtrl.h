#pragma once

/** @brief ɾ������ITEM,�ع�����ITEM */
void UILC_RebuildListCtrl( CListCtrl *listCtrl );
void UILC_InsertColumn( CListCtrl *pList );
void UILC_AddItem( CListCtrl *pList ,CSSNodeInfo *pNode ,bool bEnsureInUseVisible = TRUE );
void UILC_AddItem( CListCtrl *pList , int nItem , CSSNodeInfo *pNode ,bool bEnsureInUseVisible = TRUE );
void UILC_EditItem( CListCtrl *pList , int nItem, CSSNodeInfo *pNode );
void UILC_EditItemById( CListCtrl *pList , int nId, CSSNodeInfo *pNode );
void UILC_DeleteItem( CListCtrl *pList , int nItem );
void UILC_RefreshItems( CListCtrl *pList );
void StartGetIPLocation( void *pvoid );
void EndGetIPLocation();