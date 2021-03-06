﻿////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Реализация классов красно-черного дерева
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      01.05.2017
///            This is a part of the course "Algorithms and Data Structures" 
///            provided by  the School of Software Engineering of the Faculty 
///            of Computer Science at the Higher School of Economics.
///
/// "Реализация" (шаблонов) методов, описанных в файле rbtree.h
///
////////////////////////////////////////////////////////////////////////////////

#include <stdexcept>        // std::invalid_argument


namespace xi
{


//==============================================================================
// class RBTree::node
//==============================================================================

template<typename Element, typename Compar>
RBTree<Element, Compar>::Node::~Node()
{
    if (_left)
        delete _left;
    if (_right)
        delete _right;
}


template<typename Element, typename Compar>
typename RBTree<Element, Compar>::Node *RBTree<Element, Compar>::Node::setLeft(Node *lf)
{
    // предупреждаем повторное присвоение
    if (_left == lf)
        return nullptr;

    // если новый левый — действительный элемент
    if (lf)
    {
        // если у него был родитель
        if (lf->_parent)
        {
            // ищем у родителя, кем был этот элемент, и вместо него ставим бублик
            if (lf->_parent->_left == lf)
                lf->_parent->_left = nullptr;
            else                                    // доп. не проверяем, что он был правым, иначе нарушение целостности
                lf->_parent->_right = nullptr;
        }

        // задаем нового родителя
        lf->_parent = this;
    }

    // если у текущего уже был один левый — отменяем его родительскую связь и вернем его
    Node *prevLeft = _left;
    _left = lf;

    if (prevLeft)
        prevLeft->_parent = nullptr;

    return prevLeft;
}


template<typename Element, typename Compar>
typename RBTree<Element, Compar>::Node *RBTree<Element, Compar>::Node::setRight(Node *rg)
{
    // предупреждаем повторное присвоение
    if (_right == rg)
        return nullptr;

    // если новый правый — действительный элемент
    if (rg)
    {
        // если у него был родитель
        if (rg->_parent)
        {
            // ищем у родителя, кем был этот элемент, и вместо него ставим бублик
            if (rg->_parent->_left == rg)
                rg->_parent->_left = nullptr;
            else                                    // доп. не проверяем, что он был правым, иначе нарушение целостности
                rg->_parent->_right = nullptr;
        }

        // задаем нового родителя
        rg->_parent = this;
    }

    // если у текущего уже был один левый — отменяем его родительскую связь и вернем его
    Node *prevRight = _right;
    _right = rg;

    if (prevRight)
        prevRight->_parent = nullptr;

    return prevRight;
}


//==============================================================================
// class RBTree
//==============================================================================

template<typename Element, typename Compar>
RBTree<Element, Compar>::RBTree()
{
    _root = nullptr;
    _dumper = nullptr;
}

template<typename Element, typename Compar>
RBTree<Element, Compar>::~RBTree()
{
    // грохаем пока что всех через корень
    if (_root)
        delete _root;
}


template<typename Element, typename Compar>
void RBTree<Element, Compar>::deleteNode(Node *nd)
{
    // если переданный узел не существует, просто ничего не делаем, т.к. в вызывающем проверок нет
    if (nd == nullptr)
        return;

    // потомков убьет в деструкторе
    delete nd;
}


template<typename Element, typename Compar>
void RBTree<Element, Compar>::insert(const Element &key)
{
    // этот метод можно оставить студентам целиком
    Node *newNode = insertNewBstEl(key);

    // отладочное событие
    if (_dumper)
        _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_BST_INS, this, newNode);

    rebalance(newNode);

    // отладочное событие
    if (_dumper)
        _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_INSERT, this, newNode);

}


template<typename Element, typename Compar>
const typename RBTree<Element, Compar>::Node *RBTree<Element, Compar>::find(const Element &key)
{
    //put a pointer to the root of the tree
    Node *node = _root;

    //cycle for running through the tree
    while (node)
    {
        /** as the left child is less than the parent, and the right one is more,
         then for finding we will need 2 checks and if neither one is fulfilled,
         then we have found the necessary element */
        if (key < node->getKey())
        {
            node = node->getChild(true);    //go to the left child
        } else
        {
            if (key > node->getKey())   //if you hit this block, the key is greater than or equal to
            {
                node = node->getChild(false);  //if you hit this block, then the key is larger, so go to the right child
            } else
            {
                return node;    //if you hit this block, the key is found
            }

        }
    }
    return nullptr;
}

template<typename Element, typename Compar>
typename RBTree<Element, Compar>::Node *
RBTree<Element, Compar>::insertNewBstEl(const Element &key)
{
    //create element to insert
    Node *newElement = new Node(key, nullptr, nullptr, nullptr, RED);

    //if the tree is empty, then add the root
    if (isEmpty())
    {
        _root = newElement;

        //conditionally make the root black
        newElement->_color = BLACK;
        return newElement;
    }

    //Create parently for parent
    Node *parent = nullptr;

    //pointer to the root
    Node *node = _root;

    //loop to find a place to insert
    while (node)
    {
        /** It works on the same principle as find, but if you find such a key,
         then we throw an exception. It is also necessary to remember the parent
         in order to define our new node by the left or right child. */
        parent = node;
        if (key < node->getKey())
        {
            node = node->getChild(true);    //go to the left child
        } else
        {
            if (key > node->getKey())   //if you hit this block, the key is greater than or equal to
            {
                node = node->getChild(false);  //if you hit this block, then the key is larger, so go to the right child
            } else
            {
                throw std::invalid_argument("Key already exist");    //if you hit this block, the key is found
            }
        }
    }

    //add our new element to parent
    if (key < parent->getKey())
        parent->setLeft(newElement);    //if the key is smaller, then we put it with the left child
    else
        parent->setRight(newElement); //if greater or equal - right child

    return newElement;
}


template<typename Element, typename Compar>
typename RBTree<Element, Compar>::Node *
RBTree<Element, Compar>::rebalanceDUG(Node *nd)
{
}


template<typename Element, typename Compar>
void RBTree<Element, Compar>::rebalance(Node *nd)
{
    Node *temp;
    //as long as the parent is red
    while (nd->_parent != nullptr && nd->_parent->isRed())
    {
        //check if the parent is the left child
        if (nd->_parent == nd->_parent->_parent->_left)
        {
            //then remember uncle
            temp = nd->_parent->_parent->_right;
            //if uncle is red case 1
            if (temp != nullptr && temp->_color == RED)
            {
                //we paint parents black
                nd->_parent->_color = BLACK;
                //Uncle paint in black
                temp->_color = BLACK;
                //grandpa in red
                nd->_parent->_parent->_color = RED;
                //grandpa becomes current node
                nd = nd->_parent->_parent;
            } else
            {   //if the node is the right child
                if (nd == nd->_parent->_right)
                {
                    //then left turn зфкуте
                    nd = nd->_parent;
                    rotLeft(nd);
                }
                //we paint parents black
                nd->_parent->_color = BLACK;
                //grandpa in red
                nd->_parent->_parent->_color = RED;
                //turn right relative to grandfather
                rotRight(nd->_parent->_parent);
            }
        } else
        {
            //left uncle
            temp = nd->_parent->_parent->_left;
            //and if he is red
            if (temp != nullptr && temp->_color == RED)
            {
                //parent to black
                nd->_parent->_color = BLACK;
                //uncle in black
                temp->_color = BLACK;
                //grandpa in red
                nd->_parent->_parent->_color = RED;
                //current node - grandfather
                nd = nd->_parent->_parent;
            } else
            {
                //if the node is the left child
                if (nd == nd->_parent->_left)
                {
                    //then right turn relative to father
                    nd = nd->_parent;
                    rotRight(nd);
                }
                //parent to black
                nd->_parent->_color = BLACK;
                //grandpa in red
                nd->_parent->_parent->_color = RED;
                //left turn relative to grandfather
                rotLeft(nd->_parent->_parent);
            }
        }

    }
    _root->setBlack();
}


template<typename Element, typename Compar>
void RBTree<Element, Compar>::rotLeft(typename RBTree<Element, Compar>::Node *nd)
{
    Node *y = nd->_right;

    if (!y)
        throw std::invalid_argument("Can't rotate left since the right child is nil");

    Node *tempLeftChild = y->getChild(true);
    //make the left child y right at nd
    nd->setRight(tempLeftChild);
    //then we make nd a parent of the left child y  //UPD: Check if children have y
    if (tempLeftChild != nullptr)
        tempLeftChild->_parent = nd;

    //now parent nd must be made parent y
    if (nd->_parent == nullptr) //if nd was root
    {
        _root = y;
    } else
    {

        //if the child is left, then put the parent nd of the left descendant y, otherwise
        if (nd->isLeftChild())
            nd->_parent->setLeft(y);
        else
            nd->_parent->setRight(y);
    }
    //now nd - a child y
    y->setLeft(nd);
    //now y - parent nd
    if (nd != nullptr)
        nd->_parent = y;

    // отладочное событие
    if (_dumper)
        _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_LROT, this, nd);

}


template<typename Element, typename Compar>
void RBTree<Element, Compar>::rotRight(typename RBTree<Element, Compar>::Node *nd)
{
    // left потомок, который станет после right поворота "выше"
    Node *y = nd->_left;

    if (!y)
        throw std::invalid_argument("Can't rotate left since the right child is nil");

    Node *tempRightChild = y->getChild(false);
    //make the right child y left at nd
    nd->setLeft(tempRightChild);
    //then we make nd a parent of the right child y   //UPD: Check if children have y
    if (tempRightChild != nullptr)
        tempRightChild->_parent = nd;

    //now parent nd must be made parent y
    if (nd->_parent == nullptr) //if nd was root
    {
        _root = y;
    } else
    {

        //if the child is right, then put the parent nd of the tight descendant y, otherwise
        if (nd->isRightChild())
            nd->_parent->setRight(y);
        else
            nd->_parent->setLeft(y);
    }
    //now nd - a child y
    y->setRight(nd);
    //now y - parent nd
    nd->_parent = y;

    // отладочное событие
    if (_dumper)
        _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_RROT, this, nd);

}

template<typename Element, typename Compar>
typename RBTree<Element, Compar>::Node *RBTree<Element, Compar>::findForRemove(const Element &key)
{
    Node *node = _root;

    if (node == nullptr)
        throw std::invalid_argument("node is nullptr");
    //cycle for running through the tree
    while (node)
    {
        /** as the left child is less than the parent, and the right one is more,
         then for finding we will need 2 checks and if neither one is fulfilled,
         then we have found the necessary element */
        if (key < node->getKey())
        {
            node = node->getChild(true);    //go to the left child
        } else
        {
            if (key > node->getKey())   //if you hit this block, the key is greater than or equal to
            {
                node = node->getChild(
                        false);  //if you hit this block, then the key is larger, so go to the right child
            } else
            {
                return node;    //if you hit this block, the key is found
            }

        }
    }
    return nullptr;
}

template<typename Element, typename Compar>
void RBTree<Element, Compar>::remove(const Element &key)
{
    Node *tempChildChild, *tempChildNodeForRemove;

    Node *tempNode = findForRemove(key);

    //throw an exception if the node is not found
    if (tempNode == nullptr)
        throw std::invalid_argument("Key not find");

    //check if one of the children does not exist, assign the value of the temp node
    if (tempNode->_left == nullptr || tempNode->_right == nullptr)
    {
        tempChildNodeForRemove = tempNode;

    } else
    {
        //if not found, then go to the end of the right descendant
        tempChildNodeForRemove = tempNode->_right;
        while (tempChildNodeForRemove->_left != nullptr)
            tempChildNodeForRemove = tempChildNodeForRemove->_left;
    }

    //Check if tempChildNodeForRemove is the only descendant
    if (tempChildNodeForRemove->_left != nullptr)  //and remember the existing descendant
        tempChildChild = tempChildNodeForRemove->_left;
    else
        tempChildChild = tempChildNodeForRemove->_right;

    //check that everyone had parents and they were not zero
    if (tempChildChild != nullptr && tempChildNodeForRemove != nullptr && tempChildChild->_parent != nullptr &&
        tempChildNodeForRemove->_parent)
        tempChildChild->_parent = tempChildNodeForRemove->_parent; //and remove parents tempChildNodeForRemove

    //if he has parents
    if (tempChildNodeForRemove->_parent != nullptr)
    {
        //if the left child is the left parent
        if (tempChildNodeForRemove == tempChildNodeForRemove->_parent->_left)
            tempChildNodeForRemove->_parent->_left = tempChildChild; //then we assign the child's tempChildNodeForRemove value to the left parent
        else
            tempChildNodeForRemove->_parent->_right = tempChildChild; //otherwise
    } else
        _root = tempChildChild; //if there were no parents, then this is the root


    //if tempChildNodeForRemove is not equal to the node to be deleted
    if (tempChildNodeForRemove != tempNode)
        tempNode->_key = tempChildNodeForRemove->_key; //then in the node we assign the value of the child

    //if the node to remove is black, then rebalance
    if (tempChildNodeForRemove->_color == BLACK){
        if(tempChildChild != nullptr)
            rebalance(tempChildChild);
    }

    //delete
    free(tempChildNodeForRemove);
}

} // namespace xi

