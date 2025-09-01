import requests
import re
import os

# --- Configurações ---
# Lista de IDs de livros do Projeto Gutenberg (grandes clássicos em inglês)
# A lista foi expandida novamente para garantir que o corpus atinja o tamanho alvo.
BOOK_IDS = [
    # Lista Original
    '2701',  # Moby Dick
    '2600',  # War and Peace
    '1342',  # Pride and Prejudice
    '1184',  # The Count of Monte Cristo
    '100',   # The Complete Works of William Shakespeare
    '4300',  # Ulysses
    '1661',  # The Adventures of Sherlock Holmes
    '98',    # A Tale of Two Cities
    '84',    # Frankenstein
    '135',   # Les Misérables (English translation)
    '76',    # Adventures of Huckleberry Finn
    '5200',  # Metamorphosis
    '174',   # The Picture of Dorian Gray
    '345',   # Dracula
    '1400',  # Great Expectations
    '25344', # The Scarlet Letter
    '74',    # The Adventures of Tom Sawyer
    '12',    # Alice's Adventures in Wonderland
    '16328', # Beowulf
    '219',   # Heart of Darkness
    '2591',  # The Canterbury Tales
    '30254', # The Prince by Nicolo Machiavelli
    '120',   # Treasure Island
    '46',    # A Christmas Carol
    '64317', # The Great Gatsby
    '43',    # The Strange Case of Dr. Jekyll and Mr. Hyde
    '514',   # Little Women
    '1260',  # Jane Eyre
    '996',   # Don Quixote
    '158',   # Emma by Jane Austen
    '6130',  # The Iliad by Homer
    '28054', # The Brothers Karamazov
    '215',   # The Call of the Wild
    '45',    # Anne of Green Gables
    '1886',  # The Aeneid
    '829',   # Gulliver's Travels
    '203',   # A Connecticut Yankee in King Arthur's Court
    '205',   # Walden, and On The Duty Of Civil Disobedience
    '730',   # Oliver Twist
    '8800',  # The Divine Comedy by Dante, Illustrated
    '160',   # The Awakening, and Selected Short Stories
    '2852',  # The Hound of the Baskervilles
    '37106', # The Wind in the Willows
    '244',   # A Study in Scarlet
    '67979', # The Way of the World
    '2814',  # The House of Mirth
    '145',   # The Mayor of Casterbridge
    '35',    # The Time Machine
    '36',    # The War of the Worlds
    '1259',  # The Age of Innocence
    '1998',  # A Journey to the Center of the Earth
    '1952',  # The Yellow Wallpaper
    '16',    # Peter Pan
    '3207',  # The Republic
    '236',   # The Jungle Book
    '1257',  # The Three Musketeers
    '1837',  # The Odyssey
    '6761',  # Crime and Punishment
    '209',   # The Turn of the Screw
    # Expansão para garantir > 100MB
    '521',   # The Life and Adventures of Robinson Crusoe
    '2542',  # A Doll's House
    '42',    # The Vicar of Wakefield
    '1250',  # An Occurrence at Owl Creek Bridge
    '1080',  # A Modest Proposal
    '28885', # The Art of War
    '105',   # Persuasion
    '140',   # The Secret Garden
    '14264', # The Man Who Was Thursday
    '1155',  # The Secret Adversary
    '61',    # The Mysterious Affair at Styles
    '55',    # The Wonderful Wizard of Oz
    '20',    # Paradise Lost
    '4217',  # The Innocence of Father Brown
    '95',    # The Last of the Mohicans
    '585',   # David Copperfield
    '1409',  # The Pickwick Papers
    '1787',  # The History of Tom Jones, a Foundling
    '62',    # A Little Princess
    '245',   # Wuthering Heights
    '31100', # The Voyage of the Beagle
    '67098', # The Beautiful and Damned
    '4280',  # The History of the Peloponnesian War
    '2641',  # A Room with a View
    '19337', # The Complete "Sherlock Holmes"
    '1064',  # The Outline of History: Being a Plain History of Life and Mankind
    '14591', # The Awakening
    '103',   # Around the World in Eighty Days
    '25525', # The Possessed (The Devils)
    '2638',  # The Idiot
    '1077',  # The History of the Decline and Fall of the Roman Empire
    '3200',  # The Memoirs of Sherlock Holmes
    '108',   # The Return of Sherlock Holmes
    '2343',  # His Last Bow
    '2344',  # The Valley of Fear
    '1635',  # The Sketch-Book of Geoffrey Crayon
    '1023',  # The Complete Poetical Works of Henry Wadsworth Longfellow
    '1727',  # The Federalist Papers
    '2265',  # The Man in the Iron Mask
    '27827', # The Vicomte de Bragelonne
    '1796',  # Leaves of Grass
    '3748',  # Twenty Years After
    '1050',  # The Interesting Narrative of the Life of Oloudah Equiano
    '1900',  # The Souls of Black Folk
    '23',    # Narrative of the Life of Frederick Douglass
    '53',    # The Adventures of Pinocchio
    '115',   # The Swiss Family Robinson
    '207',   # The Black Arrow: A Tale of the Two Roses
    '216',   # Kidnapped
    '226',   # The Thirty-Nine Steps
    '41',    # The Legend of Sleepy Hollow
    '141',   # The House of the Seven Gables
    '1513',  # The Scarlet Pimpernel
    '1934',  # The Spy
    '2183',  # The Innocents Abroad
    '2422',  # Roughing It
    '289',   # The Adventures of Gerard
    '326',   # The Sign of the Four
    '473',   # The Lost World
    '574',   # The Man Who Knew Too Much
    '600',   # Notes from the Underground
    '965',   # The Woman in White
    '972',   # The Moonstone
    '768',   # Wives and Daughters by Elizabeth Gaskell
    '1514',  # North and South by Elizabeth Gaskell
    '394',   # Cranford by Elizabeth Gaskell
    '844',   # The Importance of Being Earnest by Oscar Wilde
    '1251',  # An Ideal Husband by Oscar Wilde
    '1041',  # The Canterville Ghost by Oscar Wilde
    '779',   # Lady Windermere's Fan by Oscar Wilde
    '7370',  # The Common Law by Oliver Wendell Holmes Jr.
    '5740',  # The Kama Sutra of Vatsyayana
    '2500',  # Siddhartha by Hermann Hesse
    '1228',  # The Confidence-Man by Herman Melville
    '15',    # Moby-Dick; or, The Whale (alternate)
    '10',    # The Bible, King James Version, Complete
    '30',    # The Bible, Douay-Rheims, Complete
    '18247', # The Koran (Al-Qur'an)
    '2800',  # The Book of Mormon
    '1688',  # The Problems of Philosophy by Bertrand Russell
    '5500',  # An Inquiry into the Nature and Causes of the Wealth of Nations by Adam Smith
    '3300',  # An Enquiry Concerning Human Understanding by David Hume
    '4363',  # The Essays of Montaigne, Complete by Michel de Montaigne
    '19942', # Theologico-Political Treatise by Benedict de Spinoza
    '3825',  # The Consolation of Philosophy by Boethius
    '2400',  # The Essays of Francis Bacon
    '1786',  # The Life of Johnson by James Boswell
    '3296',  # The Confessions of Jean-Jacques Rousseau
]

# Nome do arquivo de saída e tamanho alvo em Megabytes
OUTPUT_FILENAME = "english_corpus.txt"
TARGET_SIZE_MB = 100
TARGET_SIZE_BYTES = TARGET_SIZE_MB * 1024 * 1024

def preprocess_text(text):
    """
    Aplica as regras de pré-processamento ao texto.
    1. Converte para minúsculas.
    2. Remove todos os caracteres que não sejam 'a-z' ou espaço,
       substituindo-os por um espaço para não unir palavras.
    3. Substitui sequências de espaços por um único espaço.
    """
    # 1. Converte para minúsculas
    text = text.lower()
    
    # 2. Mantém apenas letras a-z e espaços, substituindo o resto por espaço
    # A expressão regular [^a-z ] significa "qualquer caractere que NÃO seja a-z ou espaço"
    text = re.sub(r'[^a-z ]', ' ', text)
    
    # 3. Substitui múltiplos espaços por um único espaço
    # A expressão regular \s+ corresponde a uma ou mais ocorrências de caracteres de espaço
    text = re.sub(r'\s+', ' ', text)
    
    return text.strip()

def create_corpus():
    """
    Baixa e processa livros do Projeto Gutenberg até atingir o tamanho alvo.
    """
    print(f"Iniciando a criação do corpus. Alvo: {TARGET_SIZE_MB} MB.")
    
    # Abre o arquivo de saída em modo de escrita, apagando o conteúdo anterior
    with open(OUTPUT_FILENAME, "w", encoding="utf-8") as outfile:
        current_size = 0
        
        for book_id in BOOK_IDS:
            # Verifica se o tamanho alvo já foi atingido
            if current_size >= TARGET_SIZE_BYTES:
                print(f"\nTamanho alvo de {TARGET_SIZE_MB} MB atingido. Processo concluído.")
                break
                
            # Monta a URL para o arquivo de texto do livro
            url = f"https://www.gutenberg.org/ebooks/{book_id}.txt.utf-8"
            
            try:
                print(f"\nBaixando o livro ID: {book_id} de {url}...")
                response = requests.get(url, timeout=30)
                # Verifica se o download foi bem-sucedido
                response.raise_for_status() 
                
                print("Processando o texto...")
                raw_text = response.text
                
                # Aplica o pré-processamento
                cleaned_text = preprocess_text(raw_text)
                
                # Escreve o texto limpo no arquivo de saída
                outfile.write(cleaned_text)
                # Adiciona um espaço para separar o conteúdo dos livros
                outfile.write(' ')
                
                # Atualiza o tamanho atual do arquivo
                current_size = os.path.getsize(OUTPUT_FILENAME)
                current_size_mb = current_size / (1024 * 1024)
                print(f"Livro ID {book_id} processado. Tamanho atual do corpus: {current_size_mb:.2f} MB")
                
            except requests.exceptions.RequestException as e:
                print(f"Erro ao baixar o livro ID {book_id}: {e}")
            except Exception as e:
                print(f"Ocorreu um erro inesperado: {e}")

    # Mensagem final
    if current_size < TARGET_SIZE_BYTES:
        print(f"\nAviso: Todos os livros foram processados, mas o tamanho alvo não foi atingido.")
    
    print(f"Corpus final salvo em '{OUTPUT_FILENAME}' com tamanho de {os.path.getsize(OUTPUT_FILENAME) / (1024*1024):.2f} MB.")


if __name__ == "__main__":
    create_corpus()

