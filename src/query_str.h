#ifndef QUERY_STR_H
#define QUERY_STR_H

/* this DOES need captures tbqh */

const char func[] = "(function_declarator) @decl-0";

const char c_decl[] =
"(declaration \
 (storage_class_specifier)* \
  type: (_) \
  declarator: (_)) @decl-0";

/* naturally excludes function definitions inside of class -- just gotta check pattern 3 */
const char cpp_decl[] =
"(class_specifier \
 name: (type_identifier) @class-0 \
 body: ((field_declaration_list (declaration (function_declarator declarator: (identifier) @id-2) @decl-1)))) \
(class_specifier \
 name: (type_identifier) @class-0 \
 body: ((field_declaration_list (declaration (function_declarator declarator: (destructor_name)) @decl-1)))) \
(class_specifier \
 name: (type_identifier) @class-0 \
 body: ((field_declaration_list (field_declaration) @field-decl-1))) \
(declaration (storage_class_specifier)* type: (_) declarator: (_) @decl0)";

#endif /* End QUERY_STR_H */
