import re

# MC3
semantics_dict = [
    {"name": "vBaseColor", "semantic": "TEXCOORD3"},
    {"name": "vPosition", "semantic": "TEXCOORD2"},
    {"name": "vEyePosition", "semantic": "TEXCOORD5"},
    {"name": "fogFactor", "semantic": "TEXCOORD9"},
    {"name": "vFogFactor", "semantic": "TEXCOORD9"},
    {"name": "vTexCoord0", "semantic": "TEXCOORD0"},
    {"name": "vTexCoord1", "semantic": "TEXCOORD1"},
    {"name": "vTexCoord2", "semantic": "TEXCOORD2"},
    {"name": "vCoord", "semantic": "TEXCOORD0"},
    {"name": "vCoord0", "semantic": "TEXCOORD0"},
    {"name": "vCoord1", "semantic": "TEXCOORD1"},
    {"name": "vCoord2", "semantic": "TEXCOORD2"},
    {"name": "vCoord3", "semantic": "TEXCOORD3"},
    {"name": "vCoord4", "semantic": "TEXCOORD4"},
    {"name": "vCoord00", "semantic": "TEXCOORD0"},
    {"name": "vCoord01", "semantic": "TEXCOORD1"},
    {"name": "vCoord02", "semantic": "TEXCOORD2"},
    {"name": "vCoord10", "semantic": "TEXCOORD3"},
    {"name": "vCoord11", "semantic": "TEXCOORD4"},
    {"name": "vCoord12", "semantic": "TEXCOORD5"},
    {"name": "vCoord20", "semantic": "TEXCOORD6"},
    {"name": "vCoord21", "semantic": "TEXCOORD7"},
    {"name": "vCoord22", "semantic": "TEXCOORD8"},
    {"name": "vCoordBlur0", "semantic": "TEXCOORD0"},
    {"name": "vCoordBlur1", "semantic": "TEXCOORD1"},
    {"name": "vCoordBlur2", "semantic": "TEXCOORD2"},
    {"name": "vCoordBlur3", "semantic": "TEXCOORD3"},
    {"name": "vCoordDecimated", "semantic": "TEXCOORD4"},
    {"name": "vCoordSource", "semantic": "TEXCOORD5"},
    {"name": "vColor", "semantic": "TEXCOORD1"},
    {"name": "vColor0", "semantic": "COLOR0"},
    {"name": "v0c", "semantic": "COLOR0"},
    {"name": "vFlashColor", "semantic": "COLOR0"},
    {"name": "vDiffuse", "semantic": "TEXCOORD1"},
    {"name": "vCoordDiffuse", "semantic": "TEXCOORD1"},
    {"name": "vNormal", "semantic": "TEXCOORD2"},
    {"name": "vCoordLightMap", "semantic": "TEXCOORD2"},
    {"name": "vCoordReflectionMap", "semantic": "TEXCOORD3"},
    {"name": "vDebug", "semantic": "TEXCOORD4"},
    {"name": "vHalf", "semantic": "TEXCOORD3"},
    {"name": "vSpecIntensity", "semantic": "TEXCOORD4"},
    {"name": "vRimCoord", "semantic": "TEXCOORD5"},
    {"name": "vEyeDirection", "semantic": "TEXCOORD6"},
    {"name": "eyeDirection", "semantic": "TEXCOORD6"},
    {"name": "vReflection", "semantic": "TEXCOORD7"},
    {"name": "vReflectionAmount", "semantic": "TEXCOORD8"},
    {"name": "vBlurFactor", "semantic": "TEXCOORD9"},
    {"name": "vFlashFactor", "semantic": "TEXCOORD8"},
    {"name": "vGammaFactor", "semantic": "TEXCOORD7"},
    {"name": "vInvFlashFactor", "semantic": "TEXCOORD6"},
    {"name": "vBlendEdges", "semantic": "TEXCOORD3"},
    {"name": "vBlend", "semantic": "TEXCOORD2"},
]

# TODO: There are other numeric types in Cg
numeric_types = ['float', 'int']

def is_matrix(t):
    matrix_types = []
    for nt in numeric_types:
        for i in range(1,5):
            for u in range(1,5):
                matrix_types.append(f"{nt}{i}x{u}")
    if t.strip() in matrix_types:
        return True
    return False

global_vars = []

def get_semantic(n):
    for s in semantics_dict:
        if s["name"] == n:
            return s["semantic"]
    return "?SEMANTIC?"

def func_append_statement(f, s, t):
    stack = 0
    startIndex = None
    fun = ""

    for i in range(t.find(f), len(t)):
        if t[i] == '{':
            if stack == 0:
                startIndex = i + 1 # string to extract starts one index later
            
            # push to stack
            stack += 1
        elif t[i] == '}':
            # pop stack
            stack -= 1

            if stack == 0:
                fun = t[startIndex:i]
                break

    fun_new = fun + "\n\t" + s + "\n"

    if len(fun) > len(f):
        return t.replace(fun, fun_new)
    else:
        print("ERROR: Could not find function " + f)
        return text


def fetch_and_remove_varyings(text):
    pattern = re.compile(r'varying\s+(\w+)\s+([\w\]\[\d]+)\s*;[\t ]*\n?')
    res = []
    for (t, n) in re.findall(pattern, text):
        res.append({"type": t, "name": n, "semantic": get_semantic(n)})
        global_vars.append({"type": t, "name": n})

    text = re.sub(pattern, "", text)

    if "gl_Position" in text:
        res.append({"type": "float4", "name": "gl_Position", "semantic": "POSITION"})
        global_vars.append({"type": "float4", "name": "gl_Position"})

    if "gl_FragCoord" in text:
        res.append({"type": "float4", "name": "gl_FragCoord", "semantic": "WPOS"})
        global_vars.append({"type": "float4", "name": "gl_FragCoord"})

    if "gl_FragColor" in text:
        res.append({"type": "float4", "name": "gl_FragColor", "semantic": "COLOR"})
        global_vars.append({"type": "float4", "name": "gl_FragColor"})

    return text, res


def fetch_and_remove_attributes(text):
    pattern = re.compile(r'attribute\s+(\w+)\s+([\w\]\[\d]+)\s*;[\t ]*\n?')
    res = []
    for (t, n) in re.findall(pattern, text):
        res.append({"type": t, "name": n})
        global_vars.append({"type": t, "name": n})
    text = re.sub(pattern, "", text)
    return text, res

# note: uniform definitions can NOT contain precision specification at this point
def find_uniforms(text):
    pattern = re.compile(r'uniform\s+(\w+)\s+([\w\]\[\d]+)\s*;[\t ]*\n?')
    for (t, n) in re.findall(pattern, text):
        global_vars.append({"type": t, "name": n})

# find variable definitions like `float x;` or `float y = 0;`
# note: variable definitions can NOT contain precision specification at this point
def find_vars(text):
    # filter out preprocessor directives for this search
    text_temp = text
    text_temp = re.sub(r"\n[ ]*#[^\n]+", "", text_temp)

    pattern = re.compile(r'[;\}\{\)\(][\s\n]*([\w\d]+)\s+([\w\]\[\d]+)\s*[=;]')
    for (t, n) in re.findall(pattern, text_temp):
        global_vars.append({"type": t, "name": n})

def find_statements(t):
    pass
    
def remove_comments(t):
    stack = 0
    startIndex = 0

    t = re.sub(r"\n//[^\n]+", "", t)

    while '/*' in t and len(t) > 3:
        for i in range(0, len(t) - 1):
            if t[i] == '/' and t[i+1] == '*':
                if stack == 0:
                    startIndex = i
                
                # push to stack
                stack += 1
            elif t[i] == '*' and t[i+1] == '/':
                # pop stack
                stack -= 1

                if stack == 0:
                    t = t[:startIndex] + t[i+2:]
                    break
    return t

def indent_and_separate_lines(t):
    # Add newlines between statements and missing tabs
    t = re.sub(r";[\t ]*(\w)", r";\n\1", t)
    t = re.sub(r"\n    ", r"\n\t", t) #TODO: Count tab width using first expression in main()
    
    tmp = t
    while True:
        t = re.sub(r"([\t]+)([^;\n]+;)\n(\w)", r"\1\2\n\t\3", t)
        if tmp == t:
            break
        tmp = t

    return t

# We use this to correctly detect frag/vert shader when there's ##include used
def process_cg_file(content, path_prefix):
    processed_content = []
    for line in content.splitlines():
        if line.startswith("##include"):
            include_file = line.split('"')[1]  # Extract the file name from the include directive
            include_path = os.path.join(path_prefix, "common", include_file)

            if not os.path.exists(include_path):
                include_path = os.path.join(path_prefix, include_file)

            if os.path.exists(include_path):
                with open(include_path, 'r') as include_file_content:
                    processed_content.extend(include_file_content.readlines())
            else:
                print(f"Warning: {include_path} not found.")

        else:
            processed_content.append(line)

    return "\n".join(processed_content)

def tex2dbias(t):
    pattern = re.compile(r'texture2D[\t\n ]*\(')
    res = pattern.search(t)
    while res is not None:
        idx_start = res.start()
        idx_end = res.end()

        args = []
        arg_num = 0
        pos = res.end()
        stack = 1 # 1 because we matched the opening bracket already
        while (stack > 0) and (pos < len(t)):
            if t[pos] == '(':
                stack += 1
            if t[pos] == ')':
                stack -= 1
            if t[pos] == ',' and stack == 1:
                args.append(pos)
                arg_num+=1
            pos += 1
        args.append(pos-1)
        arg_num += 1

        args_text = []
        for i in range(0, arg_num):
            start_pos = (res.end()) if (i == 0) else (args[i-1]+1)
            args_text.append(str(t[start_pos:args[i]]))

        replacement = ""
        if (arg_num == 3):
            replacement = "tex2Dbias(" + str(args_text[0]) + ", float4(" + str(args_text[1]) + ", 1.0, " + str(args_text[2]) + "))"
        else:
            replacement = "tex2D("
            for i in range(0, arg_num):
                if (i != 0):
                    replacement += ", "
                replacement += args_text[i]
            replacement += ")"
        t_new = t[:idx_start] + replacement + t[pos:]
        t = t_new
        res = pattern.search(t)

    return t

def fix_includes(t):
    pattern = re.compile(r'##include\s+"([^"]+)"')

    for inc in re.findall(pattern, t):
        if inc.endswith("glsl"):
            t = t.replace(inc, inc.replace("glsl", "cg"))

    return t

def format(input, input_w_includes):
    ## Kill comments
    input = remove_comments(input)

    isFrag = "gl_FragColor" in input_w_includes

    ## Add profile comment for psp2cgc
    if isFrag:
        input = "// profile sce_fp_psp2\n\n" + input
    else:
        input = "// profile sce_vp_psp2\n\n" + input

    # Add newlines between statements and missing tabs
    input = indent_and_separate_lines(input)
    input = fix_includes(input)

    ## Remove precision specification from uniforms
    input = re.sub("uniform\s+highp", "uniform", input)
    input = re.sub("uniform\s+mediump", "uniform", input)
    input = re.sub("uniform\s+lowp", "uniform", input)

    ## Remove precision specification from attributes
    input = re.sub("attribute\s+highp", "attribute", input)
    input = re.sub("attribute\s+mediump", "attribute", input)
    input = re.sub("attribute\s+lowp", "attribute", input)

    ## Remove precision specification from varyings
    input = re.sub("varying\s+highp", "varying", input)
    input = re.sub("varying\s+mediump", "varying", input)
    input = re.sub("varying\s+lowp", "varying", input)

    ## Hack for Gameloft's GLITCH engine
    input = re.sub("uniform\s+MAXP", "uniform", input)
    input = re.sub("attribute\s+MAXP", "attribute", input)
    input = re.sub("varying\s+MAXP", "varying", input)

    ## Remove other precision specifications
    input = re.sub("highp\s+", "", input)
    input = re.sub("mediump\s+", "", input)
    input = re.sub("lowp\s+", "", input)

    ## Remove global precision specification
    input = re.sub("precision\s+float\s*;\s*\n?", "", input)

    ## Replace ivecN types with Cg equivalents
    input = input.replace("ivec4", "int4")
    input = input.replace("ivec3", "int3")
    input = input.replace("ivec2", "int2")
    
    ## Replace vecN types with Cg equivalents
    input = input.replace("vec4", "float4")
    input = input.replace("vec3", "float3")
    input = input.replace("vec2", "float2")

    ## Replace matN types with Cg equivalents
    input = input.replace("mat4", "float4x4")
    input = input.replace("mat3", "float3x3")
    input = input.replace("mat2", "float2x2")

    ## Replace sampler* types with Cg equivalents
    input = input.replace("samplerCube", "samplerCUBE")

    ## Replace texture2D(x,y) and texture2D(x,y,z) with tex2D(x,y) and tex2Dbias(x,float4(y,1.0,z))
    input = tex2dbias(input)

    ## Replace texture2DProj and textureCube initializers with Cg equivalents
    input = re.sub(r"texture2DProj[\t\n ]*\(", "tex2Dproj(", input)
    input = re.sub(r"textureCube[\t\n ]*\(", "texCUBE(", input)

    ## Replace mix() with lerp(), mod() with fmod(), fract() with frac()
    input = re.sub(r"([;}{)( \t\n=\*/\-\+])mix[\t\n ]*\(", r"\1lerp(", input)
    input = re.sub(r"([;}{)( \t\n=\*/\-\+])mod[\t\n ]*\(", r"\1fmod(", input)
    input = re.sub(r"([;}{)( \t\n=\*/\-\+])fract[\t\n ]*\(", r"\1frac(", input)

    # Make sure there is an empty line before void main() {}
    input = re.sub(r";[\s\n]*void[\s\n]+main[\s\n]*\(", ";\n\nvoid main(", input)

    ## Fix "void main(void)"
    input = re.sub("void\s+main\s*\n*\(\s*void\s*\)\s*\{", "void main() {", input)

    ## A lot of shaders seem to use FOG define, replace it with CG_FOG
    input = re.sub(r"([;}{)( \t\n=\*/\-\+])FOG([;}{)( \t\n=\*/\-\+])", r"\1CG_FOG\2", input)

    ## Make sure function definition is standardized
    input = re.sub("void\s+main\s*\n*\(\s*\)\s*\{\n?", "void main() {\n", input)

    ## For frag shaders, move varyings into main() args
    if isFrag:
        varyings = []
        input, varyings = fetch_and_remove_varyings(input)
        
        varyings_str = ''
        for v in varyings:
            if v["name"] == "gl_FragColor":
                varyings_str += "\n\t" + v["type"] + " out " + v["name"] + " : " + v["semantic"] + ","
            else:
                varyings_str += "\n\t" + v["type"] + " " + v["name"] + " : " + v["semantic"] + ","
        if (len(varyings) != 0):
            varyings_str = varyings_str[:-1] + "\n"
        input = input.replace("void main()", "void main("+varyings_str+")")

    ## For vert shaders, move attributes and varyings into main() args
    if not isFrag:
        attributes = []
        input, attributes = fetch_and_remove_attributes(input)
        varyings = []
        input, varyings = fetch_and_remove_varyings(input)

        attrs_str = ''
        for v in attributes:
            attrs_str += "\n\t" + v["type"] + " " + v["name"] + ","

        if len(varyings) == 0:
            attrs_str = attrs_str[:-1] + "\n"

        varyings_str = ''
        for v in varyings:
            varyings_str += "\n\t" + v["type"] + " out " + v["name"] + " : " + v["semantic"] + ","

        if (len(varyings) != 0):
            varyings_str = varyings_str[:-1] + "\n"

        input = input.replace("void main()", "void main("+attrs_str+varyings_str+")")

    # Ensure newline at the end of file
    if input[-1] != "\n":
        input += "\n"


    ## Save uniforms and vars to `global_vars`
    find_uniforms(input)
    find_vars(input)

    matrices = []
    for v in global_vars:
        if is_matrix(v["type"]):
            matrices.append(v)
    
    if len(matrices) > 0:
        print(f"Found {len(matrices)} matrices. Will check for multiplication operations")
        find_statements(input)

    ## Clean up excess whitespaces
    input = re.sub(r"\n[ \t]*\n", "\n\n", input)
    input = re.sub(";[ \t]*\n", ";\n", input)
    input = re.sub("\}[ \t]*\n", "}\n", input)
    input = re.sub("\{\n\n", "{\n", input)
    input = re.sub(r"\n   ([\w/])", r"\n\t\1", input)
    input = re.sub(r"(#[^\n]+\n)\n+", r"\1\n", input)
    input = re.sub(r"uniform[\t ]+(\w+)[\t ]+([\w\]\[\d]+)[\t ]*;[\t ]*\n?", r"uniform \1 \2;\n", input)

    return input

import os, fnmatch

def findReplace(directory, filePattern):
    for path, dirs, files in os.walk(os.path.abspath(directory)):
        for filename in fnmatch.filter(files, filePattern):
            targetpath = path.replace(directory, directory + "_out")
            os.makedirs(targetpath, exist_ok=True)
            targetfilepath = os.path.join(targetpath, filename.replace(".glsl", ".cg"))

            if (os.path.exists(targetfilepath)):
                print(f"skipping {targetfilepath} because it already exists")
                continue

            filepath = os.path.join(path, filename)

            with open(filepath) as f:
                s = f.read()
            s = format(s, process_cg_file(s, path))

            with open(targetfilepath, "w") as f:
                f.write(s)

if __name__ == "__main__":
    #with open("bruh/0b4aad94694c729f810f0f4c1c862460e1907f27.glsl") as f:
    #    s = f.read()
    #s = format(s)
    #with open("bruh/0b4aad94694c729f810f0f4c1c862460e1907f27.cg", "w") as f:
    #    f.write(s)
    findReplace("glsl", "*.glsl")
