import pytest
import os
import shutil
from extractor import extract_lumps

def test_extract_lumps(tmp_path):
    wad_path = os.path.abspath("sample.wad")
    output_dir = str(tmp_path / "test_output")
    
    # Ensure clean output dir
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    # Freedoom has BOSSA1 (we just verified it via script)
    extracted_files = extract_lumps(wad_path, output_dir, "BOSSA1")
    
    assert len(extracted_files) >= 1
    assert os.path.exists(os.path.join(output_dir, "BOSSA1.png"))
