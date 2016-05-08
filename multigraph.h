#ifndef MULTIGRAPH_H
#define MULTIGRAPH_H

#include <QQueue>
#include <QDebug>
#include <QVector>


template<typename NodeType, typename ArcType>
class HGraphNode;

template<typename NodeType, typename ArcType>
class HGraphArc
{
public:
    HGraphNode<NodeType, ArcType> *m_node;
    ArcType m_weight;
};

template<typename NodeType, typename ArcType>
class HGraphNode
{
public:
    typedef HGraphArc<NodeType, ArcType> Arc;
    typedef HGraphNode<NodeType, ArcType> Node;

    NodeType m_data;
    QVector<Arc> m_arcList;
    bool m_marked;

    HGraphNode (){}

    HGraphNode (NodeType p_data) : m_data(p_data) {}

    HGraphNode (const HGraphNode & rhs) {
        //shallow copy
        m_data = rhs.m_data;
        m_marked = rhs.m_marked;
        //deep copy
        for(int i=0; i<rhs.m_arcList.size (); i++)
        {
            m_arcList << rhs.m_arcList[i];
        }
    }

    void AddArc(Node* p_node, ArcType p_weight)
    {
        Arc a;
        a.m_node = p_node;
        a.m_weight = p_weight;
        m_arcList << a;
    }

    bool operator==(const Node& p_second) const {
        return m_data == p_second.m_data;
    }

    class xNoArcFound{};
    Arc* GetArc(Node* p_node)
    {
        for(int i=0; i<m_arcList.size (); i++)
        {
            if(m_arcList[i].m_node == p_node)
                return &m_arcList[i];
        }
        throw xNoArcFound();
        return NULL;
    }

    void RemoveArc(Node* p_node)
    {
        for(int i=0; i<m_arcList.size (); i++)
        {
            if(m_arcList[i].m_node == p_node)
                m_arcList.remove (i);
        }
    }
};

template<typename NodeType, typename ArcType>
class MultiGraph
{
public:
    typedef HGraphArc<NodeType, ArcType> Arc;
    typedef HGraphNode<NodeType, ArcType> Node;

    QVector<Node*> m_nodes;
    int m_count;
    MultiGraph() : m_count(0){}
    ~MultiGraph()
    {
        for(int i=0; i<m_nodes.size (); i++)
        {
            delete m_nodes[i];
        }
    }

    bool AddNode(NodeType p_data)
    {
        Node tempNode(p_data);
        for(int i=0; i<m_nodes.size (); i++){
            if(*m_nodes[i] == tempNode)
                return false;
        }

        Node *aNode = new Node(p_data);
        aNode->m_marked = false;
        m_nodes << aNode;
        m_count++;
        return true;
    }

    void RemoveArc(int p_from, int p_to)
    {
        if(m_nodes[p_from] == NULL || m_nodes[p_to] == NULL)
            return;
        m_nodes[p_from]->RemoveArc (m_nodes[p_to]);
    }

    void RemoveNode(int p_index)
    {

        if(m_nodes[p_index] == NULL)
            return;

        Arc* arc;

        for(int i=0; i<m_nodes.size (); i++){
            if(m_nodes[i] != NULL){
                arc = m_nodes[i]->getArc (m_nodes[p_index]);
                if(arc!=NULL){
                    RemoveArc (i, p_index);
                }
            }
        }
        delete m_nodes[p_index];
        m_nodes[p_index] = NULL;
        m_count--;
    }

    bool AddArc(NodeType p_from, NodeType p_to, ArcType p_weight)
    {
        int idxFrom;
        int idxTo;
        GetFromToIndices (p_from, &idxFrom, p_to, &idxTo);
//        for(int i=0; i<m_nodes.size (); i++){
//            if(*m_nodes[i] == nodeFrom) idxFrom = m_nodes.indexOf (m_nodes[i]);
//            if(*m_nodes[i] == nodeTo) idxTo = m_nodes.indexOf (m_nodes[i]);
//        }

        if (m_nodes[idxFrom] == 0 || m_nodes[idxTo] == 0)
            return false;
        //commenting this out ensures that it's possible for two nodes to have multiple arcs between them
//        if (m_nodes[idxFrom]->GetArc(m_nodes[idxTo]) != 0)
//            return false;
        m_nodes[idxFrom]->AddArc(m_nodes[idxTo], p_weight);
        return true;
    }

    Arc* GetArc(NodeType p_from, NodeType p_to)
    {
        int idxFrom;
        int idxTo;
        GetFromToIndices (p_from, &idxFrom, p_to, &idxTo);

        if (m_nodes[idxFrom] == 0 || m_nodes[idxTo] == 0)
            return NULL;

        Arc* arc = NULL;
        try{
            arc = m_nodes[idxFrom]->GetArc(m_nodes[idxTo]);
        } catch (typename Node::xNoArcFound){
            qDebug() << "no arc between"<< p_from << "and" << p_to;
        }

        return arc;
    }

    void GetFromToIndices(NodeType p_from, int *idx_from, NodeType p_to, int *idx_to)
    {
        Node nodeFrom(p_from);
        Node nodeTo(p_to);
        for(int i=0; i<m_nodes.size (); i++){
            if(*m_nodes[i] == nodeFrom) *idx_from = m_nodes.indexOf (m_nodes[i]);
            if(*m_nodes[i] == nodeTo) *idx_to = m_nodes.indexOf (m_nodes[i]);
        }
    }

    void ClearMarks()
    {
        for (int index = 0; index < m_nodes.m_size; index++) {
            if (m_nodes[index] != 0) {
                m_nodes[index]->m_marked = false;
            }
        }
    }

    void DepthFirst(Node *p_node, void (*p_process)(Node*))
    {
        if(p_node == NULL)
            return;

        p_process(p_node);
        p_node->m_marked = true;

        for(typename QVector<Arc>::Iterator itr = p_node->m_arcList.begin ();
            itr != p_node->m_arcList.end (); itr++)
        {
            if(itr->m_node->m_marked == false){
                DepthFirst (itr->m_node, p_process);
            }
        }
    }

    void BreadthFirst(Node* p_node, void (*p_process)(Node*))
    {
        if(p_node == NULL)
            return;

        QQueue<Node*> queue;

        queue << p_node;
        p_node->m_marked = true;

        while(queue.size () != 0){
            p_process(queue.front ());
            for(typename QVector<Arc>::Iterator itr = p_node->m_arcList.begin ();
                itr != p_node->m_arcList.end (); itr++)
            {
                if(itr->m_node->m_marked == false){
                    itr->m_node->m_marked = true;
                    queue << itr->m_node;
                }
            }
            queue.dequeue ();
        }
    }
};

template<typename NodeType, typename ArcType>
class MultiGraphIterator
{
public:
    typedef HGraphArc<NodeType, ArcType> Arc;
    typedef HGraphNode<NodeType, ArcType> Node;
    MultiGraphIterator(MultiGraph<NodeType,ArcType> *gr)
    {
        m_graph = gr;
    }

    void begin()
    {
        if(m_graph != NULL){
            m_node = m_graph->m_nodes.first ();
            index = 0;
        }
    }

    void operator++()
    {
        index++;
    }

    bool isValid()
    {
        return index != m_graph->m_nodes.size ();
    }

    void end()
    {
        if(m_graph != NULL){
            m_node = m_graph->m_nodes.last ();
        }
    }

    Node* operator *()
    {
        m_node = m_graph->m_nodes[index];
        return m_node;
    }

private:
    const MultiGraph<NodeType, ArcType> *m_graph;
    Node* m_node;
    int index;
};

#endif // MULTIGRAPH_H
