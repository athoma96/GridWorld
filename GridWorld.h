//
// Author: Allen Thomas
// UIC Summer 2021
// Prof. Lillis
//
// Project 2: Design and implementation of all data structures for a given specification (including runtime requirements).
//you are given the specifications for an Abstract Data Type along with runtime requirements.  
//Your job:  design and implement a class called GridWorld which meets the requirements.

// GridWorld.h file
#ifndef _GRID_WORLD_H
#define _GRID_WORLD_H

#include <vector>
#include <iostream>

using std::vector;


struct Node
{
  Node* next;
  Node* prev;
  int Id;
};

struct District
{
  int disRow;
  int disCol;
  int disId;
  int peopleAlive = 0;
  Node* tmp = nullptr;

  District(int r, int c, int i, int a, Node* t)
  {
    disRow = r;
    disCol = c;
    disId = i;
    peopleAlive = a;
    tmp = t;
  }
};

class LList
{
  public:
    Node* head = nullptr;
    Node* tail = nullptr;
    int length = 0;

};

class GridWorld {

  private:
    // private stuff goes here!
    //   typedefs
    //   data members
    //   private helper functions
    //   etc.
    int rows;
    int cols;
    int personId;
    int populationSize;
    LList** disArr;
    LList deathPool;
    vector<District> personArr;



  public:
    /**
    * constructor:  initializes a "world" with nrows and
    *    ncols (nrows*ncols districtcs) in which all 
    *    districtricts are empty (a wasteland!).
    */
    GridWorld(unsigned nrows, unsigned ncols)   
    {
      // your constructor code here!
      rows = nrows;
      cols = ncols;
      personId = 0;
      populationSize = 0;
      int i = 0;

      disArr = new LList* [nrows];

      while(i < nrows)
      {
        disArr[i] = new LList [ncols];
        i++;
      }

    }
    ~GridWorld()
    {
      // your destructor code here.
      for(int i = 0; i < rows; i++)
        delete[] disArr[i];
      
      delete[] disArr;
    }

    /*
     * function: birth
     * description:  if row/col is valid, a new person is created
     *   with an ID according to rules in handout.  New person is
     *   placed in district (row, col)
     *
     * return:  indicates success/failure
     */
    bool birth(int row, int col, int &id)
    {
      if(row < 0 || col < 0 || row >= rows || col >= cols)
        return false;

      if(deathPool.length == 0)
      {
        Node* ptr = new Node();
        ptr->Id = personId;
        ptr->next = nullptr;
        ptr->prev = nullptr;


        if(disArr[row][col].head != nullptr)
        {
          disArr[row][col].tail->next = ptr;
          ptr->prev = disArr[row][col].tail;
          disArr[row][col].tail = ptr;
        }
        else
        {
          disArr[row][col].head = ptr;
          disArr[row][col].tail = ptr;
        }

        id = personId;
        personArr.push_back(District(row, col, id, 1, disArr[row][col].tail));
        personId++;
      }
      else
      {
        Node* ptr = deathPool.head;
        ptr->Id = deathPool.head->Id;

        if(ptr->next != nullptr)
        {
          deathPool.head = ptr->next;
          ptr->next->prev = nullptr;
          ptr->next = nullptr;
        }
        else
        {
          deathPool.head = nullptr;
          deathPool.tail = nullptr;
        }

        

        if(disArr[row][col].head != nullptr)
        {
          disArr[row][col].tail->next = ptr;
          ptr->prev = disArr[row][col].tail;
          disArr[row][col].tail = ptr;
        }
        else
        {
          disArr[row][col].head = ptr;
          disArr[row][col].tail = ptr;
        }

        id = ptr->Id;
        District* d = &personArr.at(id);
        d->disRow = row;
        d->disCol = col;
        d->peopleAlive = 1;
        d->tmp = ptr;

        deathPool.length--;

      }

      disArr[row][col].length++;
      populationSize++;
      return true;
    }

    /*
     * function: death 
     * description:  if given person is alive, person is killed and
     *   data structures updated to reflect this change.
     *
     * return:  indicates success/failure
     */
    bool death(int personID)
    {
      if(personID < 0 || personID >= personArr.size() || populationSize == 0)
        return false;

      District* dist = &personArr.at(personID);
      Node* ptr = dist->tmp;

      int r = dist->disRow;
      int c = dist->disCol;
      dist->peopleAlive = 0;

      if (ptr->next == nullptr && ptr->prev == nullptr)
      {
        disArr[r][c].head = nullptr;
        disArr[r][c].tail = nullptr;
      }
      else if(ptr->next == nullptr)
      {
        disArr[r][c].tail = ptr->prev;
        ptr->prev->next = nullptr;
        ptr->prev = nullptr;
      }
      else if (ptr->prev == nullptr)
      {
        disArr[r][c].head = ptr->next;
        ptr->next->prev = nullptr; 
        ptr->next = nullptr;
      }
      else
      {
        ptr->next->prev = ptr->prev;
        ptr->prev->next = ptr->next;
        ptr->next = ptr->prev = nullptr;
      }

      Node* newNode = ptr;
      newNode->Id = ptr->Id;
      newNode->next = nullptr;
      newNode->prev = nullptr;

      if(deathPool.head != nullptr && deathPool.tail != nullptr)
      {
        deathPool.tail->next = newNode;
        newNode->prev = deathPool.tail;
        deathPool.tail = newNode;
      }
      else
      {
        deathPool.head = newNode;
        deathPool.tail = newNode;
      }

      deathPool.length++;
      disArr[r][c].length--;
      populationSize--;
      return true;
      
    }

    /*
     * function: whereis
     * description:  if given person is alive, his/her current residence
     *   is reported via reference parameters row and col.
     *
     * return:  indicates success/failure
     */
    bool whereis(int id, int &row, int &col)const
    {
      if(id < 0 || id >= personId)
        return false;

      if(personArr[id].peopleAlive == 0)
        return false;
      else
      {
        row = personArr[id].disRow;
        col = personArr[id].disCol;
        return true;
      }
    }

    /*
     * function: move
     * description:  if given person is alive, and specified target-row
     *   and column are valid, person is moved to specified district and
     *   data structures updated accordingly.
     *
     * return:  indicates success/failure
     *
     * comment/note:  the specified person becomes the 'newest' member
     *   of target district (least seniority) --  see requirements of members().
     */
    bool move(int id, int targetRow, int targetCol)
    {
      if(id < 0 || id >= personId || personArr.at(id).peopleAlive == 0 || targetCol < 0 || targetCol >= cols 
      || targetRow < 0 || targetRow >= rows)
        return false;

      District* p = &personArr.at(id);
      Node* ptr = p->tmp;

      int r = p->disRow;
      int c = p->disCol;
      p->peopleAlive = 1;

      if (ptr->next == nullptr && ptr->prev == nullptr)
      {
        disArr[r][c].head = nullptr;
        disArr[r][c].tail = nullptr;
      }
      else if(ptr->next == nullptr)
      {
        disArr[r][c].tail = ptr->prev;
        ptr->prev->next = nullptr;
        ptr->prev = nullptr;
      }
      else if (ptr->prev == nullptr)
      {
        disArr[r][c].head = ptr->next;
        ptr->next->prev = nullptr; 
        ptr->next = nullptr;
      }
      else
      {
        ptr->next->prev = ptr->prev;
        ptr->prev->next = ptr->next;
        ptr->next = ptr->prev = nullptr;
      }

      Node* newNode = ptr;
      newNode->Id = ptr->Id;
      newNode->next = nullptr;
      newNode->prev = nullptr;

      if(disArr[targetRow][targetCol].head != nullptr && disArr[targetRow][targetCol].tail != nullptr)
      {
        disArr[targetRow][targetCol].tail->next = newNode;
        newNode->prev = disArr[targetRow][targetCol].tail;
        disArr[targetRow][targetCol].tail = newNode;
      }
      else
      {
        disArr[targetRow][targetCol].head = newNode;
        disArr[targetRow][targetCol].tail = newNode;
      }

      disArr[targetRow][targetCol].length++;
      disArr[r][c].length--;
      p->disRow = targetRow;
      p->disCol = targetCol;
    
      return true;
      
    }

    

    std::vector<int> * members(int row, int col)const
    {
      if(row < 0 || col < 0 || row >= rows || col >= cols)
        return nullptr;

      vector<int> * member = new vector<int>();
      Node* ptr = disArr[row][col].head;

      for(; ptr != nullptr; ptr = ptr->next)
        member->push_back(ptr->Id);

      return member;
    }

    /*
     * function: population
     * description:  returns the current (living) population of the world.
     */
    int population()const{
      return populationSize;
    }
    
    /*
     * function: population(int,int)
     * description:  returns the current (living) population of specified
     *   prict.  If district does not exist, zero is returned
     */
    int population(int row, int col)const
    {
      if(row >= rows || col >= cols || row < 0 || col < 0)
        return -1;

      return disArr[row][col].length;
    }

    /*
     * function: num_rows
     * description:  returns number of rows in world
     */
    int num_rows()const {
      return rows;
    }

    /*
     * function: num_cols
     * description:  returns number of columns in world
     */
    int num_cols()const {
      return cols;
    }



};

#endif
