# -*- coding:UTF-8 -*-
import xmltodict
import json
import os
import re
import codecs


node_transformation = 'transformation'
node_job = 'job'

ext_ktr = '.ktr'
ext_kjb = '.kjb'


def get_file_content(file_path):
    with codecs.open(file_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    # print(lines)
    return "".join(lines)


def save_file_content(file_path, content):
    with codecs.open(file_path, 'w', encoding='utf-8') as f:
        f.write(content)
    return True


def fix_xml(node, xml_dict):
    if xml_dict[node].has_key('connection') and isinstance(xml_dict[node]['connection'], dict):
        xml_dict[node]['connection'] = [xml_dict[node]['connection']]

    if node == node_transformation and isinstance(xml_dict[node]['step'], dict):
        xml_dict[node]['step'] = [xml_dict[node]['step']]


# 替换连接信息
def change_detail(node, xml_dict, config):
    if xml_dict[node].has_key('connection'):
        for i, con_detail in enumerate(xml_dict[node]['connection']):
            for pair in config['details']:
                is_match = True
                for key, value in pair['old'].items():
                    # print(key)
                    # print(con_detail)
                    if (value != con_detail[key]):
                        is_match = False
                        break
                if is_match:
                    # 用新配置替换老配置
                    for key, value in pair['new'].items():
                        xml_dict[node]['connection'][i][key] = value
                    break


# 替换连接名称
def change_name(node, xml_dict, config):
    key = 'name'
    if xml_dict[node].has_key('connection'):
        for i, con_detail in enumerate(xml_dict[node]['connection']):
            for pair in config['names']:
                if pair['old'] == con_detail[key]:
                    xml_dict[node]['connection'][i][key] = pair['new']
                    break

    key = 'connection'
    if node == node_transformation:
        for i, step in enumerate(xml_dict[node]['step']):
            for pair in config['names']:
                print key,pair
                print 'step', step
                if step.has_key(key) and pair['old'] == step[key]:
                    xml_dict[node]['step'][i][key] = pair['new']
                    break

    if node == node_job:
        for i, entry in enumerate(xml_dict[node]['entries']['entry']):
            for pair in config['names']:
                if key in entry and pair['old'] == entry[key]:
                    xml_dict[node]['entries']['entry'][i][key] = pair['new']
                    break


# 更新脚本中的数据库连接
def do_change_db(ext, old_script_path, new_script_path, config_path):
    config = json.loads(get_file_content(config_path))
    content = get_file_content(old_script_path)

    if not is_empty_string(content):
        xml_dict = xmltodict.parse(content)

        if ext_ktr == ext:
            fix_xml(node_transformation, xml_dict)
            change_detail(node_transformation, xml_dict, config)
            change_name(node_transformation, xml_dict, config)
        if ext_kjb == ext:
            fix_xml(node_job, xml_dict)
            change_detail(node_job, xml_dict, config)
            change_name(node_job, xml_dict, config)

        save_file_content(new_script_path, xmltodict.unparse(xml_dict, pretty=True))


# 批量更新
def change_batch(dir_path, config_path):
    file_names = os.listdir(dir_path)
    for file_name in file_names:
        f, ext = os.path.splitext(file_name)
        print(f, ext)
        if ext_ktr == ext or ext_kjb == ext:
            file_path = os.path.join(dir_path, file_name)
            print('parsing:', file_path)
            do_change_db(ext, file_path, file_path, config_path)





def is_empty_string(s):
    if re.match(r'^\s*$', s):
        return True
    else:
        return False

if __name__ == '__main__':
    print '开始替换kettle脚本里面的IP地址，下面一个是配置文件地址，一个是你要替换的kettle的脚本的地址、'
  
    dir_path = ''
    config_path = ''

    change_batch(dir_path, config_path)
    print '地址替换结束了'

