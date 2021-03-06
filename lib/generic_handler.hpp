#ifndef PYOSMIUM_GENERIC_HANDLER_HPP
#define PYOSMIUM_GENERIC_HANDLER_HPP

#include <boost/python.hpp>

#include <osmium/area/assembler.hpp>
#include <osmium/area/multipolygon_collector.hpp>
#include <osmium/handler.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/index/map/all.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/visitor.hpp>


typedef osmium::index::map::Map<osmium::unsigned_object_id_type, osmium::Location> index_type;

class BaseHandler : public osmium::handler::Handler {

protected:
    enum pre_handler {
        no_handler,
        location_handler,
        area_handler
    };

public:
// handler functions
virtual void node(const osmium::Node&) const = 0;
virtual void way(const osmium::Way&) const = 0;
virtual void relation(const osmium::Relation&) const = 0;
virtual void changeset(const osmium::Changeset&) const = 0;
virtual void area(const osmium::Area&) const = 0;


private:
void apply_with_location(osmium::io::Reader &r, const std::string &idx) {
    const auto& map_factory = osmium::index::MapFactory<osmium::unsigned_object_id_type, osmium::Location>::instance();
    std::unique_ptr<index_type> index = map_factory.create_map(idx);
    osmium::handler::NodeLocationsForWays<index_type> location_handler(*index);
    location_handler.ignore_errors();
    osmium::apply(r, location_handler, *this);
}

void apply_with_area(osmium::io::Reader &r,
                     osmium::area::MultipolygonCollector<osmium::area::Assembler> &collector,
                     const std::string &idx) {
    const auto& map_factory = osmium::index::MapFactory<osmium::unsigned_object_id_type, osmium::Location>::instance();
    std::unique_ptr<index_type> index = map_factory.create_map(idx);
    osmium::handler::NodeLocationsForWays<index_type> location_handler(*index);
    location_handler.ignore_errors();

    osmium::apply(r, location_handler, *this,
                  collector.handler([this](const osmium::memory::Buffer& area_buffer) {
                       osmium::apply(area_buffer, *this);
                  })
                 );
}


protected:
void apply(const std::string &filename, osmium::osm_entity_bits::type types,
           pre_handler pre = no_handler,
           const std::string &idx = "sparse_mem_array") {

    switch (pre) {
    case no_handler:
        {
            osmium::io::Reader reader(filename, types);
            osmium::apply(reader, *this);
            reader.close();
            break;
        }
    case location_handler:
        {
            osmium::io::Reader reader(filename, types);
            apply_with_location(reader, idx);
            reader.close();
            break;
        }
    case area_handler:
        {
            osmium::area::Assembler::config_type assembler_config;
            osmium::area::MultipolygonCollector<osmium::area::Assembler> collector(assembler_config);

            osmium::io::Reader reader1(filename);
            collector.read_relations(reader1);
            reader1.close();

            osmium::io::Reader reader2(filename);
            apply_with_area(reader2, collector, idx);
            reader2.close();
            break;
        }
    }
}



};

using namespace boost::python;

struct SimpleHandlerWrap: BaseHandler, wrapper<BaseHandler> {

    void node(const osmium::Node& node) const {
        if (override f = this->get_override("node"))
            f(boost::ref(node));
    }

    void default_node(const osmium::Node&) const {
    }

    void way(const osmium::Way& way) const {
        if (override f = this->get_override("way"))
            f(boost::ref(way));
    }

    void default_way(const osmium::Way&) const {
    }

    void relation(const osmium::Relation& rel) const {
        if (override f = this->get_override("relation"))
            f(boost::ref(rel));
    }

    void default_relation(const osmium::Relation&) const {
    }

    void changeset(const osmium::Changeset& cs) const {
        if (override f = this->get_override("changeset"))
            f(boost::ref(cs));
    }

    void default_changeset(const osmium::Changeset&) const {
    }

    void area(const osmium::Area& area) const {
        if (override f = this->get_override("area"))
            f(boost::ref(area));
    }

    void default_area(const osmium::Area&) const {
    }

    void apply_file(const std::string &filename, bool locations = false,
                    const std::string &idx = "sparse_mem_array")
    {
        osmium::osm_entity_bits::type entities = osmium::osm_entity_bits::nothing;
        BaseHandler::pre_handler handler = locations?
                                            BaseHandler::location_handler
                                            :BaseHandler::no_handler;

        if (this->get_override("area"))
        {
            entities = osmium::osm_entity_bits::object;
            handler = BaseHandler::area_handler;
        } else {
            if (locations || this->get_override("node"))
                entities |= osmium::osm_entity_bits::node;
            if (this->get_override("way"))
                entities |= osmium::osm_entity_bits::way;
            if (this->get_override("relation"))
                entities |= osmium::osm_entity_bits::relation;
        }

        if (this->get_override("changeset"))
            entities |= osmium::osm_entity_bits::changeset;

        apply(filename, entities, handler, idx);
    }
};

#endif
