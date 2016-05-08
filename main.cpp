//#include "widget.h"
#include "multigraph.h"
#include <QApplication>
#include <QDebug>

template<class NodeType, class ArcType>
void cout(HGraphNode<NodeType, ArcType> *node){
    qDebug() << node->m_data;
}

int main(int argc, char *argv[])
{
    //    QApplication a(argc, argv);
    //    Widget w;
    //    w.show();

    MultiGraph<char, int> gr;
    gr.AddNode ('q');
    gr.AddNode ('w');
    gr.AddNode ('e');
    gr.AddNode ('r');
    gr.AddNode ('t');
    gr.AddNode ('y');
    gr.AddArc ('q', 't', 11);
    gr.AddArc ('q', 't', 51);
    gr.AddArc ('t', 'e', 1);
    gr.AddArc ('e', 'r', 2);
    gr.AddArc ('r', 'y', 3);
    gr.AddArc ('y', 'q', 3);

    HGraphNode<char, int> *aNode;
    MultiGraph<char,int>::Arc* arc = gr.GetArc ('r', 'y');
    if(arc){
        aNode = arc->m_node;
        gr.BreadthFirst (aNode, cout);
    }

    qDebug() << "";
    MultiGraphIterator<char,int> itr(&gr);
    for(itr.begin (); itr.isValid () ; ++itr){
        qDebug() << (*itr)->m_data;
    }


    //    gr.BreadthFirst (aNode, cout);

    //    return a.exec();
}
