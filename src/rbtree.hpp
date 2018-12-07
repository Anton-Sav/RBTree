////////////////////////////////////////////////////////////////////////////////
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
    //           |node| |father| |grandfather|    |check which child is the father and return uncle|
    Node *uncle = nd->_parent->_parent->getChild(!nd->_parent->isRightChild());

    // если дядя такой же красный, как сам нод и его папа...
    if (uncle->isRed())
    {
        // дядю и папу красим в черное
        // а дедушку — в коммунистические цвета
        uncle->setBlack();
        nd->_parent->setBlack();
        nd->_parent->_parent->setRed();

        //after all repainted we go higher and put a pointer to grandfather
        nd = nd->_parent->_parent;

        // отладочное событие
        if (_dumper)
            _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_RECOLOR1, this, nd);

        // теперь чередование цветов "узел-папа-дедушка-дядя" — К-Ч-К-Ч, но надо разобраться, что там
        // с дедушкой и его предками, поэтому продолжим с дедушкой
        //..
    }// дядя черный
    else
    {
        // смотрим, является ли узел "правильно-правым" у папочки

        //if the parent of the node is a right child and the node itself is left, then we make a right turn
        if (nd->_parent->isRightChild() && nd->isLeftChild())
        {
            nd = nd->_parent;
            rotRight(nd);
        }
        //if the parent of the node is the left child and the node itself is right, then we make a left turn
        if (nd->_parent->isLeftChild() && nd->isRightChild())
        {
            nd = nd->_parent;
            rotLeft(nd);
        }


        //after turning repaint parent to black
        nd->_parent->setBlack();
        // отладочное событие
        if (_dumper)
            _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_RECOLOR3D, this, nd);


        // деда в красный
        nd->_parent->_parent->setRed();
        // отладочное событие
        if (_dumper)
            _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_RECOLOR3G, this, nd);

        //check if the parent is a right child
        if (nd->_parent->isRightChild())
            rotLeft(nd->_parent->_parent); //we turn

        //check if the parent is a left child
        if (nd->_parent->isLeftChild())
            rotRight(nd->_parent->_parent);
    }

    return nd;
}


template<typename Element, typename Compar>
void RBTree<Element, Compar>::rebalance(Node *nd)
{
    // TODO: метод реализуют студенты

    //...

    // Пока папа цвета пионерского галстука, действуем
    while (nd->_parent != nullptr && nd->_parent->isRed())
    {
        // локальная перебалансировка семейства "папа, дядя, дедушка" и повторная проверка
        nd = rebalanceDUG(nd);
    }

    //We paint a root in black by condition
    _root->setBlack();
}


template<typename Element, typename Compar>
void RBTree<Element, Compar>::rotLeft(typename RBTree<Element, Compar>::Node *nd)
{
    // правый потомок, который станет после левого поворота "выше"
    Node *y = nd->_right;

    if (!y)
        throw std::invalid_argument("Can't rotate left since the right child is nil");

    Node *tempLeftChild = y->getChild(true);
    //make the left child y right at nd
    nd->setRight(tempLeftChild);
    //then we make nd a parent of the left child y
    tempLeftChild->_parent = nd;

    //now parent nd must be made parent y
    if (nd->_parent == nullptr) //if nd was root
    {
        _root = y;

        // отладочное событие
        if (_dumper)
            _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_LROT, this, nd);
        return;

    }

    //if the child is left, then put the parent nd of the left descendant y, otherwise
    if (nd->isLeftChild())
        nd->_parent->setLeft(y);
    else
        nd->_parent->setRight(y);

    //now nd - a child y
    y->setLeft(nd);
    //now y - parent nd
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
    //then we make nd a parent of the right child y
    tempRightChild->_parent = nd;

    //now parent nd must be made parent y
    if (nd->_parent == nullptr) //if nd was root
    {
        _root = y;

        // отладочное событие
        if (_dumper)
            _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_LROT, this, nd);
        return;

    }

    //if the child is right, then put the parent nd of the tight descendant y, otherwise
    if (nd->isRightChild())
        nd->_parent->setRight(y);
    else
        nd->_parent->setLeft(y);

    //now nd - a child y
    y->setRight(nd);
    //now y - parent nd
    nd->_parent = y;

    // отладочное событие
    if (_dumper)
        _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_RROT, this, nd);

}


} // namespace xi

