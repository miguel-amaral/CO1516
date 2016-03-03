#include <iostream>
#include <forward_list>
#include <list>

class Element {
public:
	virtual void render() = 0;
};

class Page : public Element {
private:
	std::list<std::shared_ptr<Element>> _elements;
public:
	void addElement(std::shared_ptr<Element> element){
		_elements.push_back(element);
	}
	void render(){
		std::cout << "Na pagina esta: " << std::endl;
		for (std::shared_ptr<Element> element : _elements) {
			std::cout << "> " ;
			element->render() ;
			std::cout << std::endl;
		}
		std::cout << "the end!" << std::endl;
	}
};


class Text : public Element {
private:
	std::string _text;
public:
	Text(std::string text ) {
		_text = text;
	}
	void render(){
		std::cout << _text << std::endl;
	}
};

int main(){
	std::shared_ptr<Page> p1 = std::make_shared<Page>();
	std::shared_ptr<Page> p2 = std::make_shared<Page>();

	std::shared_ptr<Text> t1 = std::make_shared<Text>("t1");
	std::shared_ptr<Text> t2 = std::make_shared<Text>("t2");
	std::shared_ptr<Text> t3 = std::make_shared<Text>("t3");
	std::shared_ptr<Text> t4 = std::make_shared<Text>("t4");
	std::shared_ptr<Text> t5 = std::make_shared<Text>("t5");

	p1->addElement(p2);
	p2->addElement(t5);
	p2->addElement(t4);
	p2->addElement(t3);

	p1->addElement(t1);
	p1->addElement(t2);
	p1->addElement(t2);

	p1->render();
	return 0;
}
