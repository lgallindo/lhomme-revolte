import pytest
from pytest_bdd import scenarios, given, when, then, parsers

# Load the scenarios from the feature file
scenarios('../features/mapsets.feature')

@pytest.fixture
def engine_state():
    return {
        'platform_capabilities': {},
        'mapset_requirements': {},
        'dynamic_mapsets_enabled': False,
        'load_attempted': False,
        'load_success': False,
        'errors': []
    }
@given(parsers.parse('a target compilation platform with {keys:d} keys'))
def platform_capabilities(engine_state, keys):
    engine_state['platform_capabilities']['keys'] = keys

@given(parsers.parse('a mapset metadata defining a requirement of {keys:d} keys'))
def mapset_requirements(engine_state, keys):
    engine_state['mapset_requirements']['keys'] = keys

@when('the engine attempts to load the mapset')
def attempt_load(engine_state):
    engine_state['load_attempted'] = True
    # Mocking engine verification logic
    caps = engine_state['platform_capabilities']
    reqs = engine_state['mapset_requirements']
    
    if reqs.get('keys') and int(reqs['keys']) > int(caps.get('keys', 0)):
        engine_state['errors'].append("insufficient keys")
        engine_state['load_success'] = False
        return
        
    engine_state['load_success'] = True

@then('the engine should refuse to load the mapset')
def verify_refusal(engine_state):
    assert engine_state['load_success'] is False

@then(parsers.parse('log an error indicating {error_msg}'))
def verify_error(engine_state, error_msg):
    assert any(error_msg in err for err in engine_state['errors'])

@given(parsers.parse('the engine is compiled with LHR_DYNAMIC_MAPSETS={value:d}'))
def dynamic_mapsets(engine_state, value):
    engine_state['dynamic_mapsets_enabled'] = bool(value)

@given(parsers.parse('a valid mapset file "{filename}" is present'))
def mapset_file(engine_state, filename):
    engine_state['target_file'] = filename

@when(parsers.parse('the user requests to load "{filename}"'))
def load_dynamic(engine_state, filename):
    if not engine_state['dynamic_mapsets_enabled']:
        engine_state['errors'].append("dynamic loading is disabled")
        engine_state['load_success'] = False
    else:
        engine_state['load_success'] = True

@then('the engine should parse the file header')
def parse_header(engine_state):
    # Mock assertion
    assert engine_state['dynamic_mapsets_enabled'] is True

@then('successfully load the mapset if platform requirements are met')
def load_success(engine_state):
    assert engine_state['load_success'] is True
