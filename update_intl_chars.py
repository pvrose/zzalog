import unicodedata

# Read the current file
with open('reference/intl_chars.txt', 'r', encoding='utf-8') as f:
	chars = f.read()

# Extract unique characters (skip whitespace)
unique_chars = []
seen = set()
for c in chars:
	if c.strip() and not c.isspace() and c not in seen:
		unique_chars.append(c)
		seen.add(c)

# Create output with character and Unicode name
output_lines = []
for c in unique_chars:
	try:
		name = unicodedata.name(c)
		output_lines.append(f'{c} - {name}')
	except ValueError:
		output_lines.append(f'{c} - UNKNOWN')

# Write back to file
with open('reference/intl_chars.txt', 'w', encoding='utf-8') as f:
	f.write('\n'.join(output_lines))

print(f'Updated {len(output_lines)} characters')
