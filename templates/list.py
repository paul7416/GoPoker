from jinja2 import Environment, FileSystemLoader

env = Environment(loader=FileSystemLoader('.'))
header_template = env.get_template('templates/list.h.jinja')
src_template = env.get_template('templates/list.c.jinja')
test_template = env.get_template('templates/test_list.c.jinja')

types = [
    {'type_name': 'uint64_list', 'c_type': 'uint64_t', 'test_values': [500, 400, 300, 600, 700, 800, 900, 1000]},
    #    {'type_name': 'uint32_list', 'c_type': 'uint32_t', 'test_values': [500, 400, 300, 600, 700, 800, 900, 1000]},
    {'type_name': 'uint16_list', 'c_type': 'uint16_t', 'test_values': [500, 400, 300, 600, 700, 800, 900, 1000]},
    {'type_name': 'uint8_list', 'c_type': 'uint8_t', 'test_values': [50, 40, 30, 60, 70, 80, 90, 100]},
    {'type_name': 'int64_list', 'c_type': 'int64_t', 'test_values': [500, 400, 300, 600, 700, 800, 900, 1000]},
    {'type_name': 'int32_list', 'c_type': 'int32_t', 'test_values': [500, 400, 300, 600, 700, 800, 900, 1000]},
    {'type_name': 'int16_list', 'c_type': 'int16_t', 'test_values': [500, 400, 300, 600, 700, 800, 900, 1000]},
    {'type_name': 'int8_list', 'c_type': 'int8_t', 'test_values': [50, 40, 30, 60, 70, 80, 90, 100]},
    {'type_name': 'double_list', 'c_type': 'double', 'test_values': [500, 400, 300, 600, 700, 800, 900, 1000]},
    {'type_name': 'bool_list', 'c_type': 'bool', 'test_values': [1, 1, 0, 1, 0, 0, 0, 1]},
]

header_output = header_template.render(types=types)
with open('lib/list.h', 'w') as f:
    f.write(header_output)

src_output = src_template.render(types=types)
with open('src/list.c', 'w') as f:
    f.write(src_output)

test_output = test_template.render(types=types)
with open('test/test_list.c', 'w') as f:
    f.write(test_output)
