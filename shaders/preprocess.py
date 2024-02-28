import os

def process_cg_file(input_path, output_path):
    with open(input_path, 'r') as infile:
        content = infile.readlines()

    processed_content = []
    for line in content:
        if line.startswith("##include"):
            include_file = line.split('"')[1]  # Extract the file name from the include directive
            include_path = os.path.join("common", include_file)

            if os.path.exists(include_path):
                with open(include_path, 'r') as include_file_content:
                    processed_content.extend(include_file_content.readlines())
            else:
                print(f"Warning: {include_path} not found.")

        else:
            processed_content.append(line)

    with open(output_path, 'w') as outfile:
        outfile.writelines(processed_content)

def process_folder(input_folder, output_folder):
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    for filename in os.listdir(input_folder):
        if filename.endswith(".cg"):
            input_path = os.path.join(input_folder, filename)
            output_path = os.path.join(output_folder, filename)
            process_cg_file(input_path, output_path)

if __name__ == "__main__":
    input_folder = "cg"
    output_folder = "cg_processed"

    process_folder(input_folder, output_folder)
