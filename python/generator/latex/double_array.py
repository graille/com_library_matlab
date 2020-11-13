import numpy as np

from pathlib import Path

def generate_table(output_directory, end_filename, iterator_top, iterator_side, data_array, options = {}):
    """
        Generate a latex array from data
        Inputs:
            - output_directory: (string|Path) Output directory path
            - end_filename: (string) Name of the output latex file (without extension)
            - iterator_top: (array) List of values of the header
            - iterator_side: (array) List of values of the side
            - data_array: (array of dimension 2) List of values such as data_array[i][j] is the value for (iterator_side[i], iterator_top[j])
            - options: (dict) options
                > format_fnt: (func) Function used to format a value of data_array in the array
                > color_func: (func) Function used to determine the color of data_array[i][j]
                > line_style: (string) Array style ('simple' for simple straight line, or 'double' for double line)
                > use_hhline: (bool) Use of hhline library (default True) 
    """

    output_directory = Path(output_directory)

    # Manage options
    format_fnt = options['format_fnt'] if "format_fnt" in options.keys() else lambda x: np.round(x, 2)
    color_func = options['color_func'] if "color_func" in options.keys() else lambda x: (1, 1, 1)
    line_style = options['line_style'].lower() if "line_style" in options.keys() else 'simple'
    iterator_top_name = options['iterator_top_name'] if "iterator_top_name" in options.keys() else None
    iterator_side_name = options['iterator_side_name'] if "iterator_side_name" in options.keys() else None

    def get_hline():
        use_hhline = options['use_hhline'] if "use_hhline" in options.keys() else True

        if use_hhline:
            if line_style == "simple":
                pattern = '|'
                pattern = pattern + '-|' * (len(iterator_top) + 1)
            elif line_style == "double":
                pattern = '||'
                pattern = pattern + '=||' * (len(iterator_top) + 1)

            return f"\\hhline{{{pattern}}}"
        else:
            if line_style == "simple":
                return "\\hline"
            elif line_style == "double":
                return "\\hline \\hline"

    def vertical_align(elt, position='center'):
        # From https://tex.stackexchange.com/questions/113022/vertical-alignment-in-tabular-cells-with-variable-height
        if position == "center":
            return "{\\noindent\\parbox[c]{\\tempwidth}{" + elt + "}}"
        elif position == "bottom":
            return "{\\noindent\\parbox[b]{\\tempwidth}{" + elt + "}}"

    # Write into latex file
    print("Define colors")
    code = [
        "\\begingroup", 
        "", 
        "\\newcolumntype{Y}{>{\\centering\\arraybackslash}X}", 
        ""
    ]

    # Manage colors
    cell_color_names = []
    cell_color_values = []
    for i in range(len(iterator_side)):
        cell_color_names.append([])
        for j in range(len(iterator_top)):
            # Get color
            R, G, B = color_func(data_array[i, j])
            R, G, B = np.round(R, 3), np.round(G, 3), np.round(B, 3)

            # Find if color has already been used
            color_exist = False
            for (elt_name, colors) in cell_color_values:
                if colors == (R, G, B):
                    cell_color_names[i].append(elt_name)
                    color_exist = True
                    break

            # If not, save the new color
            if not color_exist:
                # Generate color reference/name
                color_name = str(len(cell_color_values) + 1)
                while len(color_name) < 10:
                    color_name = '0' + color_name
                color_name = "C" + color_name

                # Save color
                cell_color_names[i].append(color_name)
                cell_color_values.append((color_name, (R, G, B)))

                code += [f"\\definecolor{{{cell_color_names[i][j]}}}{{rgb}}{{{R},{G},{B}}}"]

    print("Generate array")

    if line_style == "simple":
        tabular_config = '|c|' + 'Y|' * len(iterator_top)
    elif line_style == "double":
        tabular_config = '||c||' + 'Y||' * len(iterator_top)
    
    code += ["", "\\noindent\\begin{tabularx}{\\linewidth}{" + tabular_config + "}", get_hline()]

    # Create header row
    if iterator_top_name is None and iterator_side_name is None:
        header = "- & "
    else:
        header = "{\\backslashbox{\\small " + (iterator_side_name if iterator_side_name is not None else ("")) + "}" + \
            "{\\small " + (iterator_top_name if iterator_top_name is not None else ("")) + "}} & "
    
    for j in range(len(iterator_top)):
        header += vertical_align(f"\\textbf{{{round(iterator_top[j], 1)}}}", "center")
        if j < len(iterator_top) - 1:
            header += " & "
        else:
            header += f" \\\\ {get_hline()}"

    code += [header]

    # Content
    for i in range(len(iterator_side)):
        string = vertical_align(f"{{\\textbf{{{iterator_side[i]}}} }}", "center") + " & "
        for j in range(len(iterator_top)):
            string += vertical_align(f"{{ {{\\cellcolor{{{cell_color_names[i][j]}}}}} {{${format_fnt(data_array[i, j])}$}} }}", "center")

            if j < len(iterator_top) - 1:
                string += " & "
            else:
                string += f" \\\\ {get_hline()}"

        code += [string]

    code += ["\\end{tabularx}", "", "\\endgroup"]

    # Replace empty lines with comment line (to preserve indentation)
    for i, elt in enumerate(code):
        if elt == "":
            code[i] = "%"
    
    # Write and save file
    if not output_directory.exists():
        output_directory.mkdir(parents=True, exist_ok=True)
    
    end_filename = f'{end_filename}.tex'
    print(f"Write file {end_filename}...")
    
    with open(Path(output_directory, end_filename), 'w') as f:
        f.writelines('\n'.join(code))
