#ifndef INCLUDE_UFOREST
#define INCLUDE_UFOREST

#include <iostream>
#include "unode.h"
#include "utree.h"

class uforest: public utree {
	private:
		vector<unode *> components;

	public:
		uforest(string &newick, map<string, int> &label_map, map<int, string> &reverse_label_map) : utree(newick, label_map, reverse_label_map) {
			components = vector<unode *>();
			if (leaves.size() > 0) {
				components.push_back(leaves[0]);
				leaves[0]->set_component(0);
			}
		}
		uforest(const uforest &F) : utree(F) {
			// copy vector of pointers
			components = vector<unode *>(F.components);
			// update with the new nodes
			for(int i = 0; i < components.size(); i++) {
				components[i] = get_node(components[i]->get_label());
			}
		}
		string str() const {
			stringstream ss;
			for(int i = 0; i < components.size(); i++) {
				if (i > 0) {
					ss << " ";
				}
				unode *root = components[i];
				if (root->is_leaf()) {
					root = root->get_neighbors().front();
				}
				str_subtree(ss, root, root);
				ss << ";";
			}
			return ss.str();
		}
		friend ostream& operator<<(ostream &os, const uforest& f);

		pair<int,int> cut_edge(int x, int y) {
			unode *X, *Y;
			X = get_node(x);
			Y = get_node(y);
			if (Y->get_parent() == X && X->get_parent() != Y) {
				X = get_node(y);
				Y = get_node(x);
			}
			X->remove_neighbor(Y);
			Y->remove_neighbor(X);

			unode *Xprime = X->contract();
			unode *Yprime = Y->contract();


			if (Xprime->get_component() > -1) {
				add_component(Yprime);
				update_component(Xprime->get_component(), Xprime);
			}
			else {
				add_component(Xprime);
			}
			return make_pair(Xprime->get_label(),Yprime->get_label());
		}

		void update_component(int c, int l) {
			components[c] = get_node(l);
		}

		void update_component(int c, unode *n) {
			components[c] = n;
		}

		void add_component(unode *C) {
			C->set_component(components.size());
			components.push_back(C);
		}
};

ostream& operator<<(ostream &os, const uforest& f) {
	os << f.str();
	return os;
}

#endif