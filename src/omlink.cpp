/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 1999 Enlight Software Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//Filename    : OMLINK.CPP
//Description : Object Multiple Linker

#include <omlink.h>

//--------- Begin of function MLink::MLink ---------//
//
MLink::MLink() : DynArrayB( sizeof(MLinkNode) )
{
   cur_node = NULL;
}
//----------- End of function MLink::MLink ---------//


//--------- Begin of function MLink::linkin ---------//
//
// Create a linkage in the MLink
//
// <int&> clientLink = client's link var which point to entry point
//                     recno in MLink
// <int>  destRecno  = the recno of the destination of the linkage
// [int]  linkType   = client defined link type
//                     (default:0)
//
void MLink::linkin(int& clientLink, int destRecno, int linkType)
{
	MLinkNode linkNode;
	MLinkNode *linkNodePtr;
	int       linkNodeRecno;
	int       loopCount=0;

	err_when( destRecno <= 0 );

	//---- find the last link job and tie the next_link ----//

	if( clientLink > 0 )
	{
		linkNodeRecno = clientLink;
		linkNodePtr   = (MLinkNode*) get(linkNodeRecno);

		while(1)  // process other one to many job link
		{
			if( linkNodePtr->dest_recno == destRecno )  // duplicated linked job
				return;

         if( linkNodePtr->next_link )
         {
            linkNodeRecno = linkNodePtr->next_link;
            linkNodePtr   = (MLinkNode*) get(linkNodeRecno);
         }
         else
            break;

         //------- detect dead loop ------//

         err_when(++loopCount>1000);
      }
	}

   //----- linkin a new job link -------//

   linkNode.dest_recno = destRecno;
   linkNode.next_link  = 0;
   linkNode.link_type  = linkType;

   DynArrayB::linkin( &linkNode );

   // after linkin() memory location linklist body may be changed as mem_resize() may be called

   //------- set linked job ----------//

   if( clientLink > 0 )
   {
      // after linkin() memory location linklist body may be changed as
      // mem_resize() may be called. So we need to retrieve the linkNodePtr again

      linkNodePtr = (MLinkNode*) get(linkNodeRecno);
      linkNodePtr->next_link = recno();
   }
   else
      clientLink = recno();

   return;
}
//----------- End of function MLink::linkin ----------//



//--------- Begin of function MLink::linkout -------//
//
// Linkout a destination link from the client
//
// <int&> clientLink = client's link var which point to entry point
//                     recno in MLink
// <int>  destRecno  = the recno of the destination of the linkage
//
// return : <int> 1 - linkout successfully
//                0 - the destRecno is not found in the link
//
int MLink::linkout(int& clientLink, int destRecno)
{
	if( clientLink == 0 )   // it is possible that clientLink = 0
		return 0;            // see the case explanation in Firm::mlink_out()

   err_when( destRecno <= 0 );

   //------ fisrt node is the one to be deleted -----//

   go(clientLink);

   MLinkNode *linkNodePtr = (MLinkNode*) get();

   if( linkNodePtr->dest_recno == destRecno )
   {
      clientLink = linkNodePtr->next_link;
      DynArrayB::linkout();
      return 1;
   }

   //------- other node ----------//

   MLinkNode* parentNode;
   int        loopCount=0;

   while( linkNodePtr->next_link )
   {
      parentNode = linkNodePtr;

      go(linkNodePtr->next_link);
      linkNodePtr = (MLinkNode*) get();

      if( linkNodePtr->dest_recno == destRecno )
      {
         parentNode->next_link = linkNodePtr->next_link;
         DynArrayB::linkout();
         return 1;
      }

      //------- detect dead loop ------//

      err_when(++loopCount>1000);
   }

   return 0;
}
//----------- End of function MLink::linkout -------//


//--------- Begin of function MLink::update -------//
//
// Update a existing linkage
//
// Note : no duplication check is doned on MLink::update()
//
// <int&> clientLink = client's link var which point to entry point
//                     recno in MLink
// <int>  oldDest    = the recno of the old destination of the linkage
// <int>  newDest    = the recno of the new destination of the linkage
// [int]  linkType   = client defined new link type
//                     (default:0)
//
void MLink::update(int clientLink, int oldDest, int newDest, int newType)
{
   err_when( clientLink == 0 || newDest <= 0 );

   if( oldDest==newDest )
      return;

   go(clientLink);

   MLinkNode *linkNodePtr = (MLinkNode*) get();
   int       loopCount=0;

   while(1)
   {
      if( linkNodePtr->dest_recno == oldDest )
      {
         linkNodePtr->dest_recno=newDest;
         linkNodePtr->link_type =newType;
         return;
      }

      if( linkNodePtr->next_link == 0 )    // old destination not found
         break;

      go(linkNodePtr->next_link);
      linkNodePtr = (MLinkNode*) get();

      //------- detect dead loop ------//

		err_when(++loopCount>1000);
   }

   err_now( "Mlink::update" );     // destination item not found, linkage may be corrupted
}
//----------- End of function MLink::update -------//


//--------- Begin of function MLink::clean_link -------//
//
// Clear all linkage of the client
//
// <int&> clientLink = client's link var which point to entry point
//                     recno in MLink
//
void MLink::clean_link(int& clientLink)
{
   err_when( clientLink == 0 );

   go(clientLink);

   MLinkNode *linkNodePtr = (MLinkNode*) get();
   int       nextLink;
   int       loopCount=0;

   while(1)
   {
      nextLink = linkNodePtr->next_link;
      DynArrayB::linkout();

      if( nextLink == 0 )    // MLinkNode entry is still there
         break;              // because DynArrayB is ver B

      go(nextLink);
      linkNodePtr = (MLinkNode*) get();

      //------- detect dead loop ------//

      err_when(++loopCount>1000);
   }

   clientLink = 0;
}
//----------- End of function MLink::clean_link -------//


//--------- Begin of function MLink::check_exist ---------//
//
// Check whether the specified destination exists under the specified client
//
// <int>  clientLink = client's link var which point to entry point
//                     recno in MLink
// <int>  destRecno  = the recno of the destination of the linkage
//
// return : <int> 1 - exist
//                0 - not exist
//
int MLink::check_exist(int clientLink, int destRecno)
{
   MLinkNode *linkNodePtr;
   int       loopCount=0;

   //---- find the last link job and tie the next_link ----//

   if( clientLink > 0 )
   {
      linkNodePtr = (MLinkNode*) get(clientLink);

      while(1)  // process other one to many job link
      {
         if( linkNodePtr->dest_recno == destRecno )  // duplicated linked job
            return 1;

         if( linkNodePtr->next_link )
            linkNodePtr = (MLinkNode*) get(linkNodePtr->next_link);
         else
            break;

         //------- detect dead loop ------//

         err_when(++loopCount>1000);
      }
   }

   return 0;
}
//----------- End of function MLink::check_exist ----------//


//--------- Begin of function MLink::dest_count ---------//
//
// Count the no. of destination this client has
//
// <int>  clientLink = client's link var which point to entry point
//                     recno in MLink
//
// return : <int> destCount - no. of destinations
//
int MLink::dest_count(int clientLink)
{
   MLinkNode *linkNodePtr;
   int       destCount=0;
   int       loopCount=0;

   if( clientLink > 0 )
   {
      linkNodePtr = (MLinkNode*) get(clientLink);

      while(1)  // process other one to many job link
      {
         destCount++;

         if( linkNodePtr->next_link )
            linkNodePtr = (MLinkNode*) get(linkNodePtr->next_link);
         else
            break;

         //------- detect dead loop ------//

         err_when(++loopCount>1000);
      }
   }

   return destCount;
}
//----------- End of function MLink::dest_count ----------//


//----- Begin of function MLink::first_dest ------//
//
// Call this function to find the first destination of the client.
// Then call next_dest() to find the rest of destinations.
//
int MLink::first_dest(int clientLink)
{
   if( clientLink==0 )
      return 0;

   err_if( clientLink<1 || clientLink>size() )
      err_here();

   go(clientLink);

   cur_node = (MLinkNode*)get();  // store it in var, for next_dest() to access it later
   return cur_node->dest_recno;
}
//------- End of function MLink::first_dest ------//


//----- Begin of function MLink::next_dest ------//
//
// If next_dest() is called while first_dest() is NOT called before,
// error will happen.
//
int MLink::next_dest()
{
   err_when( !cur_node );

   if( cur_node->next_link==0 )
      return 0;

   go(cur_node->next_link);

   cur_node = (MLinkNode*) get();
   return cur_node->dest_recno;
}

//------- End of function MLink::next_dest ------//


//----- Begin of function MLink::first_node ------//
//
// Similar to first_dest() except that it return the whole MLinkNode object
// so the user can also retrieve the link_type
//
MLinkNode* MLink::first_node(int clientLink)
{
   if( clientLink==0 )
      return 0;

   err_when( clientLink<1 || clientLink>size() );

   go(clientLink);

   cur_node = (MLinkNode*)get();  // store it in var, for next_dest() to access it later
   return cur_node;
}

//------- End of function MLink::first_node ------//


//------- Begin of function MLink::next_node ------//

MLinkNode* MLink::next_node()
{
   err_when( !cur_node );

   if( cur_node->next_link==0 )
      return 0;

   go(cur_node->next_link);

   cur_node = (MLinkNode*) get();
   return cur_node;
}

//-------- End of function MLink::next_node ------//


//------- Begin of function MLink::zap ------//

void MLink::zap()
{
	DynArrayB::zap();

	cur_node = NULL;
}
//-------- End of function MLink::zap -------//

